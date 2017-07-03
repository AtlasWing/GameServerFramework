#ifndef _TT_REMOVE_REFERENCE_H_
#define _TT_REMOVE_REFERENCE_H_

namespace game
{
#define BOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) /**/
#define BOOST_MPL_AUX_LAMBDA_SUPPORT(i,name,params) /**/
#define BOOST_TT_AUX_TEMPLATE_ARITY_SPEC(i, name) /**/
#define BOOST_TT_AUX_TYPE_TRAIT_DEF1(trait,T,result) \
    template< typename T > struct trait \
    { \
        typedef result type; \
        BOOST_MPL_AUX_LAMBDA_SUPPORT(1, trait, (T)) \
    }; \
    \
    BOOST_TT_AUX_TEMPLATE_ARITY_SPEC(1, trait) \
    /**/

#define BOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(param,trait,spec,result) \
    template< param > struct trait<spec> \
    { \
        typedef result type; \
    }; \
    /**/


    template <class T>
        struct remove_rvalue_ref
        {
            typedef T type;
        };
    template <class T>
        struct remove_rvalue_ref<T&&>
        {
            typedef T type;
        };

    BOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_reference, T, typename remove_rvalue_ref<T>::type);
    BOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T, remove_reference, T&, T);

}
#endif 

#ifndef _object_INCLUDED_
#define _object_INCLUDED_

#include <algorithm>
#include <typeinfo>
# if (defined(__GNUC__) && __GNUC__ >= 3) \
                                        || defined(_AIX) \
    || (defined(__sgi) && defined(__host_mips)) \
    || (defined(__hpux) && defined(__HP_aCC)) \
    || (defined(linux) && defined(__INTEL_COMPILER) && defined(__ICC))
#  define BOOST_AUX_object_TYPE_ID_NAME
#include <cstring>
# endif

class object
{
    public: // structors

        object()
            : content(0)
        {
        }

        template<typename ValueType>
            object(const ValueType & value)
            : content(new holder<ValueType>(value))
            {
            }

        object(const object & other)
            : content(other.content ? other.content->clone() : 0)
            {
            }

        ~object()
        {
            delete content;
        }

    public: // modifiers

        object & swap(object & rhs)
        {
            std::swap(content, rhs.content);
            return *this;
        }

        template<typename ValueType>
            object & operator=(const ValueType & rhs)
            {
                object(rhs).swap(*this);
                return *this;
            }

        object & operator=(object rhs)
        {
            rhs.swap(*this);
            return *this;
        }

    public: // queries

        bool empty() const
        {
            return !content;
        }

        const std::type_info & type() const
        {
            return content ? content->type() : typeid(void);
        }

    private: 

        class placeholder
        {
            public: // structors

                virtual ~placeholder()
                {
                }

            public: // queries

                virtual const std::type_info & type() const = 0;

                virtual placeholder * clone() const = 0;

        };

        template<typename ValueType>
            class holder : public placeholder
        {
            public: // structors

                holder(const ValueType & value)
                    : held(value)
                {
                }

            public: // queries

                virtual const std::type_info & type() const
                {
                    return typeid(ValueType);
                }

                virtual placeholder * clone() const
                {
                    return new holder(held);
                }

            public: // representation

                ValueType held;

            private: // intentionally left unimplemented
                holder & operator=(const holder &);
        };

    private: // representation

        template<typename ValueType>
            friend ValueType * object_cast(object *);

        template<typename ValueType>
            friend ValueType * unsafe_object_cast(object *);

        placeholder * content;

};

class bad_object_cast : public std::bad_cast
{
    public:
        virtual const char * what() const throw()
        {
            return "game::bad_object_cast: "
                "failed conversion using game::object_cast";
        }
};

    template<typename ValueType>
ValueType * object_cast(object * operand)
{
    return operand &&
        operand->type() == typeid(ValueType) ? &static_cast<object::holder<ValueType> *>(operand->content)->held : 0;
}

    template<typename ValueType>
inline const ValueType * object_cast(const object * operand)
{
    return object_cast<ValueType>(const_cast<object *>(operand));
}

    template<typename ValueType>
ValueType object_cast(object & operand)
{
    typedef typename  game::remove_reference<ValueType>::type nonref;

    nonref * result = object_cast<nonref>(&operand);
    /*if (!result)
     *      game::throw_exception(bad_object_cast());*/
    return *result;
}

    template<typename ValueType>
inline ValueType object_cast(const object & operand)
{
    typedef typename  game::remove_reference<ValueType>::type nonref;

    return object_cast<const nonref &>(const_cast<object &>(operand));
}

    template<typename ValueType>
inline ValueType * unsafe_object_cast(object * operand)
{
    return &static_cast<object::holder<ValueType> *>(operand->content)->held;
}

    template<typename ValueType>
inline const ValueType * unsafe_object_cast(const object * operand)
{
    return unsafe_object_cast<ValueType>(const_cast<object *>(operand));
}

#endif

