# 用法

## 打印系统的include.

echo "$(gcc -print-sysroot)/include"

打印系统的include.

## string输入

这个宏GET_STRING用于从标准输入stdin读取一行字符串到str变量中,具体实现如下:

1. fflush(stdin) - 清空标准输入流的缓冲区,确保读取最新输入。
2. fgets((str), MAX, stdin) - 从标准输入读取最多MAX-1个字符到str所指向的字符数组中,遇到换行符\n或者读取MAX-1个字符后停止。 弗格
3. strcspn((str),"\n") - 搜索str中第一个出现\n的位置,即换行符位置。
4. (str)[strcspn((str),"\n")] = 0 - 在str中的换行符位置放置空字符\0,即将该行字符串截断,去除换行符。

总结:

1. 清空输入缓冲区
2. 从标准输入读取一行到str字符数组
3. 去除换行符
4. 添加字符串终止符\0

这样通过GET_STRING宏就可以方便地获取stdin的一行字符串输入到str中,最大长度为MAX-1,且去除了换行符。





```c
#include <stdio.h> 
#include <string.h>
#define LEN 10

#define GET_STRING(str,MAX) fflush(stdin); \
                            fgets((str),MAX,stdin);\
                            (str)[strcspn((str),"\n")]=0;


int main(void) {

    char str1[LEN] = {0};
    printf("plz input:");
    char *str2= ({
            GET_STRING(str1, LEN);
            printf("str1 is %s;\n", str1);
            str1;
    });

    printf("str2 is %s;\n", str2);
}
```

# OOC

在C语言中,可以通过以下两种主要方式实现多态:

1. 函数指针 + 结构体

定义一个包含函数指针的结构体,不同的对象可以通过让函数指针指向不同的函数实现多态。

```c
c CCopy code 复制代码struct Shape {
  void (*draw)(struct Shape *shape);
};

void drawTriangle(struct Shape *shape) {
  // 绘制三角形
}

void drawCircle(struct Shape *shape) {
  // 绘制圆形  
}

int main() {
  struct Shape triangle;
  triangle.draw = drawTriangle;

  struct Shape circle;
  circle.draw = drawCircle;

  triangle.draw(&triangle); // 绘制三角形
  circle.draw(&circle); // 绘制圆形
}
```

1. 虚函数表 + 继承

使用虚函数表保存对象方法的函数指针,子类对象共享父类的虚函数表并重写自己的方法。

```c
c CCopy code 复制代码struct ShapeVtab {
  void (*draw)(struct Shape *shape);
};

struct Shape {
  struct ShapeVtab *vptr;
};  

struct Triangle {
  struct Shape super;
  // 三角形特有数据
};

struct Circle {
  struct Shape super; 
  // 圆形特有数据
};

void drawTriangle(struct Shape *shape) {
  // 绘制三角形 
}

void drawCircle(struct Shape *shape) {
  // 绘制圆形
}

int main() {
  struct Triangle t;
  struct Circle c;
  t.vptr = &triangleVtab;
  c.vptr = &circleVtab;

  t.vptr->draw(&t); // 调用三角形的绘制函数
  c.vptr->draw(&c); // 调用圆形的绘制函数  
}
```

综上,C语言可以通过结构体+函数指针的方式模拟出类似面向对象的多态特性。

Copy



triangleVtab circleVtab没有定义，怎么写

F



您提醒了我,triangleVtab和circleVtab需要定义虚函数表的具体实现,完整的代码应该是:

```c
c CCopy code 复制代码// 三角形的虚函数表
struct ShapeVtab triangleVtab = {
  drawTriangle
};

// 圆形的虚函数表  
struct ShapeVtab circleVtab = {
  drawCircle  
};

struct Shape {
  struct ShapeVtab *vptr; 
};

struct Triangle {
  struct Shape super;
  // 三角形的数据
};

struct Circle {
  struct Shape super;
  // 圆形的数据
};

// 三角形和圆形的绘制函数

void drawTriangle(struct Shape *shape) {
  // 绘制三角形
}

void drawCircle(struct Shape *shape) {
  // 绘制圆形
}

int main() {

  struct Triangle t;
  struct Circle c;  

  // 设置对象的虚函数表指针
  t.vptr = &triangleVtab;
  c.vptr = &circleVtab;

  // 调用虚函数
  t.vptr->draw(&t); 
  c.vptr->draw(&c);
}
```

通过为每个类定义其虚函数表结构体,并设置对象的虚函数表指针,从而实现多态。

Copy



super变量没有用到，怎么用呢

F



您说的对,在我之前的代码示例中,Shape结构体中的super变量没有被用到。

super变量的作用是指向子类对象中包含的父类部分,这样可以通过super访问父类的数据和方法。

例如,如果要在子类的方法中访问父类的数据成员或调用父类的方法,可以这样使用super:

```c
c CCopy code 复制代码struct Shape {
  int id;
  void (*draw)(struct Shape *shape);
}

struct Circle {
  struct Shape super; // 包含父类的部分
  
  double radius;
};

void drawCircle(struct Circle *c) {
  // 通过super访问父类id
  printf("Shape id: %d\n", c->super.id); 
  
  // 调用父类的draw方法
  c->super.draw(&c->super);
  
  // 圆形特有的绘制代码
}
```

super的使用允许子类对象同时保留父类的数据,并访问父类接口,类似C++中的继承特性。

所以super变量的作用是连接子类和父类,访问父类部分的信息,从而实现继承多态。


  ## Why using templates? 为什么使用模板？

Imagine you have to write a set of functions in C, which differ only by a few keywords (typically type keywords). For instance, let's say we want to write a function computing the sum of two arrays of length n, for floats, doubles, and ints (null pointers tests are left out for the sake of clarity):
想象一下，您必须用 C 语言编写一组函数，这些函数仅存在几个关键字（通常是类型关键字）。例如，假设我们要编写一个函数来计算两个长度为 n 的数组之和，对于浮点型、双精度型和整数型（为了清楚起见，省略了空指针测试）：

```c
  void sum_float(int n, float *a, float *b)
  {
    /* computes a:=a+b where a and b are two arrays of length n */
    int i;
    for(i=0;i<n;i++) a[i]+=b[i];
  }

  void sum_double(int n, double *a, double *b)
  {
    /* computes a:=a+b where a and b are two arrays of length n */
    int i;
    for(i=0;i<n;i++) a[i]+=b[i];
  }

  void sum_int(int n, int *a, int *b)
  {
    /* computes a:=a+b where a and b are two arrays of length n */
    int i;
    for(i=0;i<n;i++) a[i]+=b[i];
  }
```

Wouldn't it be nicer to type the function just once, by specifying what keyword is "variable", then "instanciating" the function for each possible value of the "variable", and then having a way to call the exact variant of the function? Of course, for a simple function like this one, it wouldn't make much sense. But think about a much, much longer function. Or set of functions...
通过指定什么关键字是“变量”，然后为“变量”的每个可能值“实例化”该函数，然后有一种方法来调用该函数的确切变体，只键入该函数一次不是更好吗？功能？当然，对于像这样的简单函数来说，它没有多大意义。但考虑一下一个非常非常长的函数。或者一组函数...

This is what templates do. In C++ there is a `template `keyword that allows to do just that. The downsides is that it is sometimes hard to port. And it does not exist in C.
这就是模板的作用。在 C++ 中，有一个 `template `关键字可以做到这一点。缺点是有时很难移植。并且在C语言中不存在。

Here's a technique to emulate the use of templates in C. It only uses the standard C preprocessor and it is, as far as I know, ANSI C89-compliant. And it works in C++, too. It is also very useful for OpenCL, which has a fully working pre-processor but no templates.
这是一种模拟 C 语言模板使用的技术。它仅使用标准 C 预处理器，并且据我所知，它符合 ANSI C89。它也适用于 C++。它对于 OpenCL 也非常有用，它有一个完全工作的预处理器，但没有模板。

**Note**: Over the year, this simple blog post has been cited in many places, translated into Russian, quoted on Greek, Chinese, Finnish forums. Wow! I was contacted and asked whether I "discovered' this technique. No, I do not claim to have invented this trick, because I saw it while digging in code. However, I have found that it is not widely known despite its obvious usefulness, hence the tutorial.
注：一年来，这篇简单的博客文章已被许多地方引用，翻译成俄语，在希腊语、中文、芬兰语论坛上引用。哇！有人联系我并询问我是否“发现”了这项技术。不，我并不声称发明了这个技巧，因为我在挖掘代码时看到了它。然而，我发现尽管它明显有用，但它并没有广为人知，因此本教程。

## Templates in C C 中的模板

### Ingredient 1: a concatenation macro 成分 1：串联宏

First, we need to declare a couple of macros. Those macros need to be included in every file that makes use of templates. To make things easier we will declare them in a .h file called "templates.h":
首先，我们需要声明几个宏。这些宏需要包含在每个使用模板的文件中。为了让事情变得更容易，我们将在名为“templates.h”的 .h 文件中声明它们：

| **templates.h**                                              |
| ------------------------------------------------------------ |
| #ifndef TEMPLATES_H_ #ifndef 模板_H_ #define TEMPLATES_H_ #定义模板_H_  #define CAT(X,Y) X##_##Y #define CAT(X,Y) X##_##Y #define TEMPLATE(X,Y) CAT(X,Y) #定义模板(X,Y) CAT(X,Y)  #endif |

The goal of macro `TEMPLATE(X,Y)` is to have a keyword that enables us to concatenate X and Y with an underscore in between, like this: X_Y, so that writing `TEMPLATE(function,type)` may translate to `function_type`.
宏 `TEMPLATE(X,Y)` 的目标是拥有一个关键字，使我们能够将 X 和 Y 之间用下划线连接起来，如下所示：X_Y，以便编写 `TEMPLATE(function,type)` 可以翻译为 `function_type` 。

The `##` operator is a C preprocessor directive which allows to concatenate two tokens. The reason we can't use only a single `#define TEMPLATE(X,Y) X##Y` macro is that if X is itself a #def'd constant, it will not be replaced by its value (the details of this question and the details behind the hack to make it work anyway may be found [here (self-referential macros)](https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Self-Referential-Macros.html) and [here (argument prescan)](https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html)).
`##` 运算符是一个 C 预处理器指令，允许连接两个标记。我们不能仅使用单个 `#define TEMPLATE(X,Y) X##Y` 宏的原因是，如果 X 本身是一个 #def'd 常量，则它不会被它的值替换（这个问题的详细信息以及后面的详细信息）无论如何，可以在此处（自引用宏）和此处（参数预扫描）找到使其工作的 hack。

### Ingredient 2: the functions to "templatize" 成分 2：“模板化”功能

Ok, so now we want to write a .c and a .h corresponding to the functions we'd like to have as templates, right? Let's write them. To denote the variable type keyword, we use letter 'T'. This will be #defined later on in the tutorial.
好的，现在我们要编写一个 .c 和一个 .h 对应于我们想要作为模板的函数，对吧？让我们把它们写下来吧。为了表示变量类型关键字，我们使用字母“T”。这将在本教程稍后#define 中定义。

First the .h: 首先是.h：

| **sum_as_template.h**                                        |
| ------------------------------------------------------------ |
| `  #ifdef T    #include "templates.h"    void TEMPLATE(sum,T)(int n, T *a, T *b);    #endif ` |

Notice we don't guard the .h against multiple inclusion by using the standard `#ifndef HEADER_H_` stuff. This is intentional. We'll see later why. On the other hand, the `#ifdef T` test is optional, but *very* useful to guard against any unlawful inclusion in the case T isn't defined, so the compiler doesn't throw a fit and starts hissing at you.
请注意，我们没有通过使用标准 `#ifndef HEADER_H_` 内容来保护 .h 免受多重包含。这是故意的。我们稍后会看到原因。另一方面， `#ifdef T` 测试是可选的，但对于防止未定义 T 的情况下的任何非法包含非常有用，因此编译器不会大发雷霆并开始向您发出嘶嘶声。

And now the .c:
现在.c：

| **sum_as_template.c**                                        |
| ------------------------------------------------------------ |
| `  #ifdef T    #include "templates.h"    int TEMPLATE(sum,T) (int n, T *a, T *b)  {        /* computes a:=a+b where a and b are two arrays of length n */        int i;        for(i=0;i<n;i++) a[i]+=b[i];  }    #endif ` |

### Mix everything in a bowl... 将所有东西混合在一个碗里...

Now we really want to instanciate the `sum` function so that all its variants exist (`sum_float`, `sum_double`, etc). We create another set of .h and .c files:
现在我们真的想实例化 `sum` 函数，以便它的所有变体都存在（ `sum_float` 、 `sum_double` 等）。我们创建另一组 .h 和 .c 文件：

The following .c file is the one we will compile just as another .c in the project:
以下 .c 文件是我们将像项目中的另一个 .c 一样进行编译的文件：

| **all_possible_sums.c**                                      |
| ------------------------------------------------------------ |
| `  #include "templates.h"  #include "all_possible_sums.h"    #ifdef T  #undef T  #endif  #define T float  #include "sum_as_template.c"    #ifdef T  #undef T  #endif  #define T double  #include "sum_as_template.c"    #ifdef T  #undef T  #endif  #define T int  #include "sum_as_template.c" ` |

Note: on GCC, `#undef T` would have been enough without the `#ifdef T` / `#endif` around it; but Visual C++ (at least up to version 7) does not like it...
注意：在 GCC 上，如果没有 `#ifdef T` / `#endif` 周围的 `#undef T` 就足够了；但 Visual C++（至少到版本 7）不喜欢它......

The following .h is the one we'll include in any .c where a variant of the `sum_...` function is used.
以下 .h 是我们将包含在使用 `sum_...` 函数变体的任何 .c 中的文件。

| **all_possible_sums.h**                                      |
| ------------------------------------------------------------ |
| `  #ifndef ALL_POSSIBLE_SUMS_H_   #define ALL_POSSIBLE_SUMS_H_    #include "templates.h"    #ifdef T  #undef T  #endif  #define T float  #include "sum_as_template.h"    #ifdef T  #undef T  #endif  #define T double  #include "sum_as_template.h"    #ifdef T  #undef T  #endif  #define T int  #include "sum_as_template.h"    #endif ` |

This time we understand why we didn't guard sum_as_template.h against multiple inclusions: it is included once per type...
这次我们明白了为什么我们没有保护 sum_as_template.h 免受多次包含：它每个类型包含一次......

### ...and serve as it is! ...并按原样服务！

We're all set now! Let's use the templates in an example:
我们现在都准备好了！让我们在示例中使用模板：

| **main.c**                                                   |
| ------------------------------------------------------------ |
| `  #include "all_possible_sums.h"  int main(int argc, char **argv)  {    int ai[3] = {1,2,3};    int bi[3] = {4,5,6};    float af[3] = {1.0,2.0,3.0};    float bf[3] = {1.5,2.5,3.5};    TEMPLATE(sum,int)(3,ai,bi);    TEMPLATE(sum,float)(3,af,bf);    return 0;  } ` |

That's all, folks! 就这样，伙计们！

### The last word 最后一个字

What about complex types, you may ask? What if I want to make a template where "T" is to be replaced by, say, "unsigned long long"? Wouldn't the compiler complain that it does not know about "`void sum_unsigned long long()`"?
您可能会问，复杂类型怎么样？如果我想制作一个模板，其中“T”将替换为“unsigned long long”，该怎么办？编译器不会抱怨它不知道“ `void sum_unsigned long long()` ”吗？

The answer is yes, but there's a workaround: typedef the offending, "multi-worded" type into a "single-worded" type, like this:
答案是肯定的，但有一个解决方法：将有问题的“多字”类型键入“单字”类型，如下所示：

```
typedef unsigned long long uint64;
```

Then use `TEMPLATE(sum,uint64)` instead of `TEMPLATE(sum,unsigned long long)`. Voilà!
然后使用 `TEMPLATE(sum,uint64)` 而不是 `TEMPLATE(sum,unsigned long long)` 。瞧！
