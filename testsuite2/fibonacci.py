def fib(m): 
 if m<=1: return 1
 else: 
  return fib(m-1) + fib(m-2)
 end 
end

def f(x):
 if x<10: print fib(x)
  x=x+1
  f(x)
 end
end

def main():
 x = 0
 f(x)
end

