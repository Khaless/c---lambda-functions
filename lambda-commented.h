#ifndef _A2_H
#define _A2_H

/*
 * Ok, This will be a pretty heavy post on creating something similar to what Boost
 * calls its lambda library. I wrote this code back at university quite some time ago
 * and decided it would make a good topic for you all to read, so I hope you enjoy.
 *
 * The following Defines _1 to be used for constructing lambda expressions.
 * When used, this expression will be evaluated at compile time 
 * and an object will be built which represents the expression 
 * and can be executed at runtime by calling the objects functor.
 *
 * Example(s):
 *  Use STL's for_each and print out all the items in a collection
 *  for_each(iter.begin(), iter.end(), cout << _1 << endl);
 *
 *  Use STL's for_each and double all values 
 *  for_each(iter.begin(), iter.end(), _1 = _1 * 2);
 *
 *  To help you understand the benefits of this, here is a more traditional way
 *  of accomplishing something over a collection (in this case printing out
 *  double the number):
 *
 * class MyFn {
 *	public:
 *	int operator()(int &v);
 * };
 *
 * int MyFn::operator()(int &v) {
 *  cout << v * 2 << "\n";
 *	return v;
 * }
 * MyFn f;
 * for_each(v.begin(), v.end(), f);
 *
 * As you can see, not only do we define a new class but we also, in order to
 * understand what f is doing have to manually follow code back to
 * operator().
 *
 * Of course, you could also use <functional> too and do something like this:
 *
 * transform(v.begin(), v.end(), v.begin(), bind2nd(std::multiplies<int>(), 2));
 *
 * This would multiple all elements in the collection by 2... but at this point
 * many peoples heads start to hurt.
 */

/*
 * Include iostream which gives us access to the std::ostream type
 */
#include <iostream>

/*
 * IO manipulator functions are an interesting case.
 * First, let me explain what they are. When you have an expression,
 * for example cout << "something" << endl;
 * the endl is actually a function which appends \n character into your
 * output buffer and calls flush on the stream. In this case, we must store
 * an pointer to an IO manipulator function for later execution.
 * Here we are defining a type just to make our lives easier, however it should
 * be noted that I am actually most likely doing this the wrong way, as it should
 * probably be more similar to:
 * typedef std::basic_ostream<_CharT, _Traits>& (*iomanipfunc)(std::basic_ostream<_CharT, _Traits>&);
 * because ostream is actually a templated type.
 *
 * Note: for those interested, here is the definition of endl on my system:
 * template<typename _CharT, typename _Traits>
 *   inline basic_ostream<_CharT, _Traits>& 
 *   endl(basic_ostream<_CharT, _Traits>& __os)
 *   { return flush(__os.put(__os.widen('\n'))); }
 */
typedef std::ostream&(*iomanipfunc)(std::ostream&);


/*
 * We use traits here so that we handle types correctly.
 * This may be confusing at first but think of it as a tree.
 * 
 * For example, consider the expression 1 + 2 + 4.0
 * the types for this expression are <int> + <int> + <float>
 * The ideas is that given two types 
 * (type1 and type2) we want to return the type which is most appropriate for an 
 * operation combining these two types. The example above is interesting, as it
 * invloves two operations, add 1 + 2 and add 4 to the result fo 1 + 2. Clearly
 * in order to know the type of the 2nd operation we must determine the resulting of
 * the first operation. Therefor, our Trait class can, as a type have a Trait Class 
 * and so forth.
 *
 *
 * In the example, the return type should be a float - in order to determine this
 * at the top level, we used Trait classes.  *
 * The above example can be visualised as a tree with 3 leaf nodes (1, 2 and 4.0).
 * The first operation carried out is 1 + 2, the resulting type of this expression
 * becomes Trait<int, int>::Type. When the compiler looks for the type of
 * Trait< Trait<int, int>, float > it will then firstly call on the type 
 * <em>Trait<int, int>::Type</em> which, in this case will be <em>int</em>
 * causing the expression to be evaluated as <em>Trait<int, float> who's Type is defined
 * as a float. The result is, the compiler knows it should return a <em>float</em>.
 *
 *	To save our poor fingers, we define macros
 *	which are able to create our Trait template specific classes for us.
 *
 *	Two final thing to note is that for the default case, we assign
 *	the type of the LHS (left hand side) as the appropriate type.
 *	We also have a special case for ostream, when an ostream is on any side
 *	of a binary op. - Can someone tell me why? :)
 *
 */

#define MAKE_TRAITS_1(TYPE1, TYPE2, RESULTTYPE) \
template<> \
struct LambdaTrait<TYPE1, TYPE2> { \
    typedef RESULTTYPE Type; \
}; 

#define MAKE_TRAITS_2(TYPE1, TYPE2, RESULTTYPE) \
template<> \
struct LambdaTrait<TYPE1, TYPE2> { \
    typedef RESULTTYPE Type; \
}; \
template<> \
struct LambdaTrait<TYPE2, TYPE1> { \
    typedef RESULTTYPE Type; \
}; 

// Traits - default, use LHS (T1)
template <class T1, class T2>
struct LambdaTrait {
	typedef T1 Type; // use T1 as default
};

// Make additional Traits
MAKE_TRAITS_1(int,int,int); // int + int = int
MAKE_TRAITS_2(int,float,float); // int + float = float && float + int = float
MAKE_TRAITS_2(int,double,double); // etc..
MAKE_TRAITS_2(float,double,double); // etc..

// Special cases for ostreams
template <class T2>
struct LambdaTrait<std::ostream, T2> {
	typedef std::ostream& Type;
};
template <class T2>
struct LambdaTrait<T2, std::ostream> {
	typedef std::ostream& Type;
};
template <>
struct LambdaTrait<std::ostream, std::ostream> {
	typedef std::ostream& Type;
};


/*
 * We can now define our operations (sum, minus etc.) Later on
 * these will be attached to our overloaded operators.
 *
 * Note, I am using doOp here instead of a functor (operator())
 * for the object. I am doing this because operator() cannot be declared  as
 * static inline. I want to declare doOp static inline in the /hope/ 
 * that the compiler will optimise away the objects/methods. 
 *
 * Also Note the use of LambdaTrait<T1, T2>::Type to work out the appropriate type.
 *
 * Finally, see if you can work out why LambdaStreamOP has the return 
 * value it has here.
 */

template <class T1, class T2>
class LambdaAssignOP {
    public:
		typedef typename LambdaTrait<T1, T2>::Type Type;
		static inline Type doOp(T1& a, T2& b) { a = (T1)b; return a; }
		static inline Type doOp(T1& a, T2 b) { a = (T1)b; return a; }
};

template <class T1, class T2>
class LambdaSumOP {
    public:
		typedef typename LambdaTrait<T1, T2>::Type Type;
		static inline Type doOp(T1 a, T2 b) { return a + b; }
};

template <class T1, class T2>
class LambdaMinusOP {
    public:
		typedef typename LambdaTrait<T1, T2>::Type Type;
		static inline Type doOp(T1 a, T2 b) { return a - b; }
};

template <class T1, class T2>
class LambdaMultOP {
    public:
		typedef typename LambdaTrait<T1, T2>::Type Type;
		static inline Type doOp(T1 a, T2 b) { return a * b; }
};

template <class T1, class T2>
class LambdaDivOP {
	public:
		typedef typename LambdaTrait<T1, T2>::Type Type;
		static inline Type doOp(T1 a, T2 b) { return a / b; }
};

template <class T2>
class LambdaOStreamOP {
	public:
		typedef typename LambdaTrait<std::ostream, T2>::Type Type;
		static inline Type doOp(std::ostream& os, T2 b) {
			os << b;
			return os;
		}
};

/*
 * We now define the containers which can hold literals in the statement,
 * ostream's or a reference to the actual variable being passed into the
 * expression (conceptually this is  _1). For this, we define three 
 * classes:
 *
 *  - LambdaVariable: Holds the value being passed into the expression (_1)
 *									  as a reference so we can do assignment.
 *  - LambdaLiteral: Holds any literal number for later use in evaluation.
 *  - LambdaStream: Holds an ostream object for later use in evaluation.
 */

class LambdaVariable {
    public:
		typedef int Type;
		Type& operator()(Type& x) {
            return x;
        }
};

template <class T1>
class LambdaLiteral {
    private:
        T1 _l;
    public:
		typedef T1 Type;
        LambdaLiteral(const T1 x) 
            : _l(x) { }
        T1 operator()(int) {
            return _l;
        }
};

class LambdaStream {
	private:
		std::ostream& _o;
	public:
		typedef std::ostream& Type;
		LambdaStream(std::ostream& o) : _o(o) { }
		std::ostream& operator()(int) {
			return _o;
		}
};

/*
 * Now that we have our traits, operators and variables we require
 * a container to hold a binary expression. That is, an expression
 * with a left hand side, right hand side and an operator.
 *
 * This is where the BinaryExpression class comes in. It holds a 
 * Left hand side (expression), a Right hand side (expression) and 
 * an operator to apply.
 *
 * Note, while I do not define a UnaryExpression, this could easily be 
 * added.
 *
 */

template <class L, class R, class OP>
    class LambdaBinaryExpression {
        private:
            L _l;
            R _r;
        public:
			typedef typename LambdaTrait<typename L::Type, typename R::Type>::Type Type;
            LambdaBinaryExpression (const L& l, const R& r)
                : _l(l), _r(r) { }

            Type operator()(int& x) {
                return OP::doOp(_l(x), _r(x));
            }
            
    };

/*
 * At this point we are able to represent a single variable, literal or stream,
 * or a single binary expression with a LHS and RHS.
 *
 * We are however, not yet able to build actual Expressions which may contain
 * Expressions within them (sub-expressions). To highlight this point, take the 
 * following example:
 *  _1 = 1 + 1
 *
 * Here we can break this down into two expressions, for descriptive purposes 
 * I will refer to these as expr#1 and expr#2 where:
 *   expr#2 = 1 + 1 and expr#1 = _1 = expr#1
 * Now, it is clear that in order to evaluate expre#1, we also have to evaluate 
 * #expr2 which could be quite complex. In order to encapsulate this behavour
 * nicely we wrap any single Lamda-Variable/Steam/Literal/BinaryExpression
 * into a LambdaExpression. We then do not have to worry about what is inside
 * this sub-LamdaExpression, we can just be sure to call operator() on it
 * and receive the correct answer.
 *
 */
template <class E>
class LambdaExpression {
    private:
        E _expr;
    public:
		typedef typename E::Type Type;
		LambdaExpression(const E& e) 
            : _expr(e) { }

        Type operator()(int& x) {
            return _expr(x);
        }
};

/*
 * Special Case for LambdaVariable where operator() wants to return a reference.
 * Remember our LambdaVariable comes wrapped in a LambdaExpression so that
 * too must return a reference, hence the specialisation here.
 *
 * ps. A reference is needed for assignment, i.e.
 * _1 = 2
 * _1 = <LambdaExpression>
 *
 * in the case of _1 = 2 above the expression becomes (look at operator=())
 * LambdaExpression<LambdaBinaryExpression< <LambdaExpression <LambdaVariable<int > > >, LambdaExpression< LambdaLiteral<int> >, LambdaAssignOP<int > > >
 */
template <>
class LambdaExpression<LambdaVariable> {
    private:
        LambdaVariable _expr;

    public:
		typedef LambdaVariable::Type Type;

        LambdaExpression(const LambdaVariable& e) 
            : _expr(e) { }

        Type& operator()(int& x) {
            return _expr(x);
        }

        // The assign operator for LambdaExpression
        template <class R> 
            LambdaExpression<LambdaBinaryExpression<LambdaExpression<LambdaVariable>, LambdaExpression<R>, LambdaAssignOP<Type, typename R::Type> > >
            operator=(const LambdaExpression<R>& r) { 
                typedef LambdaBinaryExpression<LambdaExpression<LambdaVariable>, LambdaExpression<R>, LambdaAssignOP<Type, typename R::Type> > LambdaExpression_t; 
                return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(*this, r)); 
            }
	
        // The assign operator for literal
        template <class R>
        LambdaExpression<LambdaBinaryExpression<LambdaExpression<LambdaVariable>, LambdaExpression<LambdaLiteral<R> >, LambdaAssignOP<Type, R> > >
            operator=(const R x) { 
                typedef LambdaBinaryExpression<LambdaExpression<LambdaVariable>, LambdaExpression<LambdaLiteral<R> >, LambdaAssignOP<Type, R > > LambdaExpression_t; 
                LambdaLiteral<R> r(x);
                return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(*this, r)); 
            }
};

// Special case for lambda stream
template <>
class LambdaExpression<LambdaStream> {
    private:
        LambdaStream _expr;

    public:
		typedef LambdaStream::Type Type;
        LambdaExpression(const LambdaStream& e) 
            : _expr(e) { }

        std::ostream& operator()(int& x) {
            return _expr(x);
        }
};

// cout << EXPR
template <class R> 
	LambdaExpression<LambdaBinaryExpression<LambdaExpression<LambdaStream>, LambdaExpression<R>, LambdaOStreamOP<typename R::Type> > > 
	operator<<(std::ostream& o, const LambdaExpression<R>& r) { 
		typedef LambdaBinaryExpression<LambdaExpression<LambdaStream>, LambdaExpression<R>, LambdaOStreamOP<typename R::Type> > LambdaExpression_t; 
		LambdaStream l(o);
		return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r)); 
	};

// EXPR << literal
template <class L, class R> 
	LambdaExpression<LambdaBinaryExpression<LambdaExpression<L>, LambdaLiteral<R>, LambdaOStreamOP<R> > >
	operator<<(const LambdaExpression<L>& l, R x) { 
		typedef LambdaBinaryExpression<LambdaExpression<L>, LambdaLiteral<R>, LambdaOStreamOP<R> > LambdaExpression_t; 
		LambdaLiteral<R> r(x);
		return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r)); 
	};

// EXPT << ManipFunc
template <class L> 
	LambdaExpression<LambdaBinaryExpression<LambdaExpression<L>, LambdaLiteral<iomanipfunc>, LambdaOStreamOP<iomanipfunc> > >
	operator<<(const LambdaExpression<L>& l, iomanipfunc x) { 
		typedef LambdaBinaryExpression<LambdaExpression<L>, LambdaLiteral<iomanipfunc>, LambdaOStreamOP<iomanipfunc> > LambdaExpression_t; 
		LambdaLiteral<iomanipfunc> r(x);
		return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r)); 
	};

/*
 * We can now create our Operations in the form of a 
 * LambdaExpression<LambdaBinaryExpression...> with a left hand side, right 
 * hands side and operation.
 *
 * To save us headache and inaccuracies we use a Macro to do this. 
 * Most people feel that using macro's to generate code is a bad idea,
 * well in this case it is a superior alternative to writing it out
 * by hand :)
 */
#define CREATE_OP(LAMBDAOP, OP) \
template <class L, class R> \
    LambdaExpression<LambdaBinaryExpression<LambdaExpression<L>, LambdaExpression<R>, LAMBDAOP<typename L::Type, typename R::Type> > > \
    OP(const LambdaExpression<L>& l, const LambdaExpression<R>& r) { \
        typedef LambdaBinaryExpression<LambdaExpression<L>, LambdaExpression<R>, LAMBDAOP<typename L::Type, typename R::Type> > LambdaExpression_t; \
        return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r)); \
    }; \
 \
template <class L, class R> \
    LambdaExpression<LambdaBinaryExpression<LambdaExpression<L>, LambdaExpression<LambdaLiteral<R> >, LAMBDAOP<typename L::Type, R> > > \
    OP(const LambdaExpression<L>& l, const R x) { \
        typedef LambdaBinaryExpression<LambdaExpression<L>, LambdaExpression<LambdaLiteral<R> >, LAMBDAOP<typename L::Type, R> > LambdaExpression_t; \
        LambdaLiteral<R> r(x); \
        return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r)); \
    }; \
template <class L, class R> \
    LambdaExpression<LambdaBinaryExpression<LambdaExpression<LambdaLiteral<L> >, LambdaExpression<R>, LAMBDAOP<L, typename R::Type> > > \
    OP(const L x, const LambdaExpression<R>& r) { \
        typedef LambdaBinaryExpression<LambdaExpression<LambdaLiteral<L> >, LambdaExpression<R>, LAMBDAOP<L, typename R::Type> > LambdaExpression_t; \
        LambdaLiteral<L> l(x); \
        return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r)); \
    };

CREATE_OP(LambdaSumOP, operator+)
CREATE_OP(LambdaMinusOP, operator-)
CREATE_OP(LambdaMultOP, operator*)
CREATE_OP(LambdaDivOP, operator/)

/*
 * We may now initialize what we have created.
 * __1 will store a reference to the value which may be manipulated
 * or accessed by our lambda expression. This is a LambdaAvairable.
 *
 * However, as everything is passed and accessed wrapped inside a LambdaExpression
 * our _1 is actually a LambdaExpression wrapping the LambdaVariable.
 */
LambdaVariable __1;
LambdaExpression<LambdaVariable> _1(__1);
#endif
