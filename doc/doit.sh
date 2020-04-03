echo "Did you remember to resize all pictures to 100%?"

awk '/font-size: *$/{while (match($0,"font-size: *$")){t=$0;getline;$0=t $0;}}{print}' <stickos.htm |
sed 's!font-size: *[x13456789][x0-9.]*pt[;]*!!g' >s2.htm
mv s2.htm stickos.htm

awk '/font-size: *$/{while (match($0,"font-size: *$")){t=$0;getline;$0=t $0;}}{print}' <cpustick.htm |
sed 's!font-size: *[x13456789][x0-9.]*pt[;]*!!g' >c2.htm
mv c2.htm cpustick.htm

