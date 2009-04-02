#ifndef _A2_H
#define _A2_H

// Author: Mathew Rodley
// SID: 305126180
// Ename: mrod5488
// Email: mrod5488@usyd.edu.au
// Purpose:
// Defines _1 to be used for lambda expressions

// For ostream
#include <iostream>

// for ostream manip funcs
typedef std::ostream&(*iomanipfunc)(std::ostream&);

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

// Taits - default, use LHS (T1)
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

//
// The operator we call for an expression
// operator() cant be static inline so use doOp
//
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

// Holds a the _1 variable
class LambdaVariable {
    public:
		typedef int Type;
		Type& operator()(Type& x) {
            return x;
        }
};

//
// Class remembers a literal in the expression, i.e the 1 in _1 + 1
//
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

// holds ostream
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

//
// Binary Expression
//
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

// 
// Contains an expression (Wrapped)
//
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

// Special Case for LambdaVariable where operator() wants to return a reference
// Also LambdaVariable can have an assignment operator on the LHS
// i.e 
// _1 = 2
// _1 = <expr>
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
 * Creating an operation is standard bar which operator to overload
 * and which evaluation object to use
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

// Initialize out _1 Object, it should act like a variable
LambdaVariable __1;
LambdaExpression<LambdaVariable> _1(__1);

#endif
