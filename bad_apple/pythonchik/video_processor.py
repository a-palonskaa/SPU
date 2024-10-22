import cv2
import math

SCREEN_WIDTH = 100
SCREEN_HEIGHT = 100

VIDEO_WIDTH = 200
VIDEO_HEIGHT = 150
TARGET_FPS = 25

def calculate_color(bgr):
    blue = max(0, min(bgr[0], 255))
    green = max(0, min(bgr[1], 255))
    red = max(0, min(bgr[2], 255))
    return (red << 16) | (green << 8) | blue

def process_video(input_file, output_file):
    video_capture = cv2.VideoCapture(input_file)

    fps = int(video_capture.get(cv2.CAP_PROP_FPS))
    frame_count = int(video_capture.get(cv2.CAP_PROP_FRAME_COUNT)) - 10
    current_colors = [None] * (SCREEN_WIDTH * SCREEN_HEIGHT)

    output = []

    for frame_index in range(1, frame_count, math.ceil(fps / TARGET_FPS)):
        video_capture.set(cv2.CAP_PROP_POS_FRAMES, frame_index)
        success, frame = video_capture.read()

        if not success:
            continue

        frame = cv2.resize(frame, (VIDEO_WIDTH, VIDEO_HEIGHT), interpolation=cv2.INTER_LANCZOS4)
        square_width = VIDEO_WIDTH // SCREEN_WIDTH
        square_height = VIDEO_HEIGHT // SCREEN_HEIGHT

        for x in range(SCREEN_WIDTH):
            for y in range(SCREEN_HEIGHT):
                center_x = x * square_width + square_width // 2
                center_y = y * square_height + square_height // 2

                if center_x < VIDEO_WIDTH and center_y < VIDEO_HEIGHT:
                    bgr_color = frame[center_y, center_x]
                    color = calculate_color(bgr_color)
                    index = y * SCREEN_WIDTH + x

                    if current_colors[index] != color:
                        output.append(f"push {color}")
                        output.append(f"pop [{index}]")
                        current_colors[index] = color

        output.append("show")

    output.append("hlt")

    with open(output_file, "w") as file:
        file.write("\n".join(output))

    video_capture.release()

input_filename = 'bad_apple.mp4'
output_filename = '../bad_apple_new.txt'
process_video(input_filename, output_filename)
