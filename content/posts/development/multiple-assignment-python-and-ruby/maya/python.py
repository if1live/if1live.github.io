f = open("command.txt")
for line in f:
    line = line.strip()
    var_names = [x.strip().replace("*", "") for x in line.split("=")[0].split(",")]
    try:
        print("source: {0}".format(line))
        exec(line)

        for var in var_names:
            exec("print('{0}=' + str({0}))".format(var))
    except Exception as e:
        print("{0}: {1}".format(e.__class__.__name__, str(e)))
    print("")
