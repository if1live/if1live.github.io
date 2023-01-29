WIN_MODE_FULLSCREEN = 1 << 0
WIN_MODE_STENCIL = 1 << 1
WIN_MODE_DOUBLE_BUFFER = 1 << 2

def create_window(mode):
    if mode & WIN_MODE_FULLSCREEN == WIN_MODE_FULLSCREEN:
        print("fullscreen");

    if mode & WIN_MODE_STENCIL == WIN_MODE_STENCIL:
        print("stencil");

    if mode & WIN_MODE_DOUBLE_BUFFER == WIN_MODE_DOUBLE_BUFFER:
        print("double buffer")


if __name__ == "__main__":
    create_window(WIN_MODE_FULLSCREEN | WIN_MODE_DOUBLE_BUFFER)
