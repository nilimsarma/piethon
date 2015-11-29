def pow(x,y):
 return x**y
end

def calcpow2(m):
 if m>=0:
  print pow(2,m)
  calcpow2(m-1)
 end
end

def main():
 x=10
 calcpow2(x)
end
