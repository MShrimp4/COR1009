import mido
import re
import time

songg = [4,4,5,5,6,6,7,7,8,2,2,3]
def mid2port(mid):
    port = int(mid) % 12
    return songg[port]


note_on = re.compile("^note_on")
note_prop = re.compile("[.0-9]+")
mid = mido.MidiFile('song.mid')
port = mido.open_output()
mlist = list()
last = [1,2,0]
count = 0
crazy = False

for msg in mid:
    args = note_prop.findall(str(msg))
    if note_on.match(str(msg)):
        count = count + 1
        tieme= int(float(args[3])*0.96*1000000)
        if tieme == 0 :
            tieme = last[2]
            last = [last[0],last[1],0]
            
        mlist.append(",".join(list(map(lambda x: str(x), last))))
        if int(args[2]) == 0 :
            last = ["1",mid2port(args[1]),tieme]
        else :
            last = ["0",mid2port(args[1]),tieme]


            
mlist.append(",".join(list(map(lambda x: str(x), last))))
mlist.append("2,0,0")
array = "{ {%s} }" % "},{".join(mlist)
with open("sketch_hell.txt",'w') as f:
    f.write("musicData notes[%d]=%s"%((count+2),array)) ## 마지막 줄 더 붙임.
