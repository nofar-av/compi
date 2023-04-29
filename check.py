import os
totalFailed = 0
totalPassed = 0
failed = []
for i in range(1, 57):
    cmd = f"./a.out < ./tests/input/t{i}.in > ./myOutput{i}.out"
    os.system(cmd)
    cmd = f"diff -s ./tests/expected/t{i}.out ./myOutput{i}.out && echo $?"
    result = os.popen(cmd).read()[-2]
    if result == "0":
        totalPassed += 1
    else:
        failed.append(i)
        totalFailed += 1
    os.system(cmd)
print ("totalPassed:", totalPassed, "|", "totalFailed:",totalFailed)
print("Failed:", failed)