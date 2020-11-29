#!/bin/python3

import random
import os
import filecmp

settings = {
    'maxLen': 10,
    'filepath': 'TEST'
}

def genTest():

    strLen = random.randint(1, settings['maxLen'])

    chars = range(ord('a'), ord('z') + 1)

    with open(settings['filepath'], 'w') as file:
        for ch in range(strLen):
            val = random.choice(chars)
            file.write(chr(val))
        file.write('\n')
        


if __name__ == "__main__":
    
    iter = 0
    while True:
        genTest()

        print('Generated test' + str(iter))
        iter += 1
        
        os.system("./old <TEST >old.txt")
        os.system("./new <TEST >new.txt")
        
        if filecmp.cmp('old.txt', 'new.txt') == False:
            print('Bad test found')
            break 

    
