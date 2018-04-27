# Promise模拟
这个工程使用c++模拟js中的promise机制  

# 使用方法
- 创建promise
```
Promise([](Resolver r, Rejector j){...});
Promise([](Resolver r){...});
Promise([](){...});
```
- then/catch
整体语法为：
```
promise.then(onResolve, onReject);
//下面两个等价
promise.then(onResolve);
promise.then(onResolve,nullptr);
//下面两个等价
promise.then(nullptr, onReject);
promise.Catch(onReject);
```
onResolve的语法为：
```
T onResolve();
T onResolve(Any a);
//T可以为任意值，甚至是Promise或者void
```

onReject的语法为：
```
T onReject();
T onReject(std::exception_ptr ptr);
//同理，T可以为Promise或者void
```