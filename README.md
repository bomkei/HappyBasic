Easy and Simple BASIC language.

join developers server!!
https://discord.gg/gjtxXcsFgx

`Warning`
**Now project is busy very much.
Cannot use some statements, and master branch is have some debugs.
Please wait until calm down.**

example:
```
def fibo(n) {
  if( n < 2 )
    return 1;
  
  return fibo(n - 2) + fibo(n - 1);
}

print(fibo(10));
```

statements:
```
if
for
while
switch
do-while
break
continue
return
```

user-defined function
```
def MyFunc(a, b, c) {
  return [a, b, c] * 2;
}

def AAA(...)
  print(get_args());
```

switch
```
switch( 1 + 2 ) {
  case 0:
    print("this language is trash");
    break;
  
  case 4 to 100:
    print("Huh!?");
    break;
  
  case 3:
    print("Yeah");
    break;
}
```

struct
```
struct People {
  var name = "Steve";
  var age = 15;
  var width = 1000000;
  var height = 3.14;
}
```

class
```
class MyClass {
  class SubClass {
    var A = 10;
    var B = 20;
    
    def fun()
      return A + B;
  }
  
  var a = 0;
  var b = 0;
  var c = new SubClass();
  
  def yeah()
    return a + b + c.A;
}

a = new MyClass();
print(a.yeah())
```
