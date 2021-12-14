import numpy as np


def read_yuv420p_frame(file_name, dims):
    with open(file_name, "rb") as f:
        h, w = dims
        buffer = f.read(int(h * w * 3 / 2))
        img = np.frombuffer(buffer, dtype=np.uint8).reshape((int(h * 1.5), w))
        return img


yuv420p_file_path = "./video.yuv420p"
yuv420p_img = read_yuv420p_frame(yuv420p_file_path, (1920, 1080))
print(yuv420p_img)
