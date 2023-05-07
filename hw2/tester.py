import os
import subprocess
TEST_INPUTS = "./in/"
TEST_ACTUAL = "./actual/"
TEST_EXPECTED = "./expected/"

RED = "\033[0;31m"
GREEN = "\033[0;32m"
RESET = "\033[0m"

if __name__ == "__main__":
    print("Tester for compi! HW2")
    print("Current directory: ", os.getcwd())
    print("Found", len(os.listdir(TEST_INPUTS)), "tests.\n")
    if 'actual' not in os.listdir(os.getcwd()):
        os.mkdir('actual')
    print("== Compiling ==")
    command = "make"
    print(command, "\n")
    subprocess.call(command, shell=True)

    print("== Testing ==")
    for input in os.listdir(TEST_INPUTS):
        input_file = TEST_INPUTS+input
        actual_file = TEST_ACTUAL+input.split(".")[0]+".out"
        expected_file = TEST_EXPECTED+input.split(".")[0]+".out"
        with open('_tmpmyfile', 'w') as checkfile:
            command = "./hw2 < "+input_file+" > "+actual_file
            print(RESET + "Running test", input.split(".")[0], ": ")
            subprocess.call(command, shell=True, stdout=None)

            command = "diff "+expected_file+" "+actual_file
            subprocess.call(command, shell=True, stdout=checkfile)
            checkfile.seek(0, os.SEEK_END)
            if checkfile.tell():
                print(RED + "FAIL")
            else:
                print(GREEN + "PASS")
            checkfile.truncate(0)
    os.remove("_tmpmyfile")
