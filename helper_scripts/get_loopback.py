import os

def is_loopback_free(loop_num):
    return os.system(f'losetup /dev/loop{loop_num} > /dev/null 2>&1') != 0

def find_lowest_consecutive_free():
    for i in range(255):
        if is_loopback_free(i) and is_loopback_free(i+1):
            return i
    return None

if __name__ == "__main__":
    lowest = find_lowest_consecutive_free()
    if lowest is not None:
        print(lowest)
    else:
        print("ERROR!")