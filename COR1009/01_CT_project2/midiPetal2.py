import mido
import re
import time


note_on = re.compile("^note_on")
note_prop = re.compile("[.0-9]+")
mid = mido.MidiFile('song.mid')
port = mido.open_output()
mlist = list()
last = [1,2,0]
count = 0
crazy = False
least = 999999
best = 0

for msg in mid:
    args = note_prop.findall(str(msg))
    if note_on.match(str(msg)):
        count = count + 1
        tieme= int(float(args[3])*0.97*1000000)
        if tieme == 0 :
            tieme = last[2]
            last = [last[0],last[1],0]
            
        mlist.append(",".join(list(map(lambda x: str(x), last))))
        mnum = int(args[1])
        if int(args[2]) == 0 :
            least = min(mnum,least)
            best = max(mnum,best)
            last = ["1",mnum,tieme]
        else :
            last = ["0",mnum,tieme]


            
mlist.append(",".join(list(map(lambda x: str(x), last))))
mlist.append("2,0,0")
array = "{ {%s} }" % "},{".join(mlist)
with open("sketch_hell.txt",'w') as f:
    f.write("musicData notes[%d]=%s;"%((count+2),array)) ## 마지막 줄 더 붙임.
print("%d~%d"%(least,best))
