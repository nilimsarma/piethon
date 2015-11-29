def main():
x=-1
y=7
z=2
if x<=y:
 if y<=z:
  print x
  print y
  print z
 else:
  if x<=z:
   print x
   print z
   print y
  else:
   print z
   print x
   print y
  end
 end
else:
 if z<=y:
  print z
  print x
  print y
 end 
end
if y<=x:
 if x<=z:
  print y
  print x
  print z
 else:
  if y<=z:
   print y
   print z
   print x
  end
 end
end
end
