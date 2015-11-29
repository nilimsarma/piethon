def calcsum(p):
 if p==1:
  return 1
 else:
  return p+calcsum(p-1)
 end
end

def sum(m):
 if m<20:
  print calcsum(m)
  sum(m+1)
 end
end
def main():
 sum(1)
end
