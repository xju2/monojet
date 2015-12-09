#!/usr/bin/env python

def read_grl(file_name):
    lumi_dic = {}
    with open(file_name, 'r') as f:
        ikey = 0
        ivalue = 5
        iblock = 11
        icount = 0
        key = 10
        value = 20
        for line in f:
            if "Total" in line:
                key = "all"
            elif icount == iblock*ikey:
                #print line[4:10]
                key = int(line[4:10])
            elif icount == iblock*ikey + ivalue:
                value = float(line[4:9])
                lumi_dic[key] = value
                ikey += 1
            else:
                pass
            icount += 1
    for key in sorted(lumi_dic.keys()):
        print "{}: {:.2f},".format(key, lumi_dic[key])

if __name__ == "__main__":
    read_grl("grl.html")
