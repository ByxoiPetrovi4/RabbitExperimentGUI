#ifndef QSHEADER_H
#define QSHEADER_H

#include <functional>
#include <map>
#include <vector>
#include <tuple>
#include <variant>
#include <chrono>
#include <iterator>
#include <typeinfo>
#include <experimental/type_traits>
#include <iostream>

#include <QVariant>
#include <QObject>
#include <qmetaobject.h>

#define LABEL
#define USE_CHLIT using namespace std::chrono_literals;

//NEED to move this to cmake define
#define QS_WIDGETS

#if defined(_WIN32) || defined(_WIN64)
    #define QS_FS_DELIMETR    '\\'
    #define QS_DATASETS_DIR QString("E:\\Programming\\DataSets\\QStager\\")
#endif
#if defined(__linux__)
    #define QS_FS_DELIMETR    '/'
    #define QS_DATASETS_DIR QString("/home/roger/Programming/DataSets/QStager/")
#endif

///none_t structure use as alternative to void in default template args
struct none_t{};



template <typename Key>
inline bool inHInterval(Key k1, Key k2, Key value)
{
    if(k1 > k2)
    {
        return value <= k1 && value > k2;
    }
    else
    {
        return value <= k2 && value > k1;
    }
}

///githublink: https://github.com/AlexKiryushkin/rttg
///runtime tuple author: AlexKiryushkin
namespace rt_tuple
{
    template <class T>
    struct ToVariantRef;

    template <class T>
    using ToVariantRefT = typename ToVariantRef<T>::type;

    template <class... T>
    struct ToVariantRef<const std::tuple<T...>>
    {
     using type = std::variant<std::reference_wrapper<std::add_const_t<T>>...>;
    };

    template <class... T>
    struct ToVariantRef<std::tuple<T...>>
    {
     using type = std::variant<std::reference_wrapper<T>...>;
    };

    template <class TupleTRef>
    auto switchCaseGetImpl(TupleTRef&& tuple, std::size_t index)
        -> ToVariantRefT<std::remove_reference_t<TupleTRef>>
    {
        using TupleT = std::remove_reference_t<TupleTRef>;
        using ReturnT = ToVariantRefT<std::remove_reference_t<TupleTRef>>;

        static_assert(std::tuple_size_v<TupleT> < 100);

    #define CASE_N(I)                                                                \
        case (I):                                                                    \
        if constexpr ((I) < std::tuple_size_v<TupleT>)                               \
        {                                                                            \
            return ReturnT{ std::in_place_index_t<I>{},                              \
                            std::ref(std::get<I>(std::forward<TupleTRef>(tuple))) }; \
        }                                                                            \
        else                                                                         \
        {                                                                            \
            break;                                                                   \
        }
    //at least its fast
        switch (index)
        {
            CASE_N(0);
            CASE_N(1);
            CASE_N(2);
            CASE_N(3);
            CASE_N(4);
            CASE_N(5);
            CASE_N(6);
            CASE_N(7);
            CASE_N(8);
            CASE_N(9);
            CASE_N(10);
            CASE_N(11);
            CASE_N(12);
            CASE_N(13);
            CASE_N(14);
            CASE_N(15);
            CASE_N(16);
            CASE_N(17);
            CASE_N(18);
            CASE_N(19);
            CASE_N(20);
            CASE_N(21);
            CASE_N(22);
            CASE_N(23);
            CASE_N(24);
            CASE_N(25);
            CASE_N(26);
            CASE_N(27);
            CASE_N(28);
            CASE_N(29);
            CASE_N(30);
            CASE_N(31);
            CASE_N(32);
            CASE_N(33);
            CASE_N(34);
            CASE_N(35);
            CASE_N(36);
            CASE_N(37);
            CASE_N(38);
            CASE_N(39);
            CASE_N(40);
            CASE_N(41);
            CASE_N(42);
            CASE_N(43);
            CASE_N(44);
            CASE_N(45);
            CASE_N(46);
            CASE_N(47);
            CASE_N(48);
            CASE_N(49);
            CASE_N(50);
            CASE_N(51);
            CASE_N(52);
            CASE_N(53);
            CASE_N(54);
            CASE_N(55);
            CASE_N(56);
            CASE_N(57);
            CASE_N(58);
            CASE_N(59);
            CASE_N(60);
            CASE_N(61);
            CASE_N(62);
            CASE_N(63);
            CASE_N(64);
            CASE_N(65);
            CASE_N(66);
            CASE_N(67);
            CASE_N(68);
            CASE_N(69);
            CASE_N(70);
            CASE_N(71);
            CASE_N(72);
            CASE_N(73);
            CASE_N(74);
            CASE_N(75);
            CASE_N(76);
            CASE_N(77);
            CASE_N(78);
            CASE_N(79);
            CASE_N(80);
            CASE_N(81);
            CASE_N(82);
            CASE_N(83);
            CASE_N(84);
            CASE_N(85);
            CASE_N(86);
            CASE_N(87);
            CASE_N(88);
            CASE_N(89);
            CASE_N(90);
            CASE_N(91);
            CASE_N(92);
            CASE_N(93);
            CASE_N(94);
            CASE_N(95);
            CASE_N(96);
            CASE_N(97);
            CASE_N(98);
            CASE_N(99);
            CASE_N(100);
        }
    #undef CASE_N
    }

    template <class TupleTRef>
    auto switchCaseGet(TupleTRef&& tuple, std::size_t index)
        -> ToVariantRefT<std::remove_reference_t<TupleTRef>>
    {
        using TupleT = std::remove_reference_t<TupleTRef>;

        if (index >= std::tuple_size_v<TupleT>)
        {
            throw std::out_of_range("Error! Tuple index is out of range!\n");
        }

        return rt_tuple::switchCaseGetImpl(std::forward<TupleTRef>(tuple), index);
    }

    template <typename T>
    uint16_t tupleSize(T value)
    {
        return std::tuple_size<T>{};
    }
} // namespace runtime tuple

namespace qs {
    ///type used in library for timestamps and other time purposes
    typedef qint64      TimeT;
    typedef int32_t     FrameT;

    struct Color24Bit
    {
        uchar   b;
        uchar   g;
        uchar   r;
    };

    const int           QSVideoMax = 64;

    inline QString getFileDirectory(QString filename)
    {
        return filename.section(QS_FS_DELIMETR, 0, -2) + QS_FS_DELIMETR;
    }

    inline QString getFileDirectoryQ(QString filename)
    {
        return filename.section('/', 0, -2) + '/';
    }

    inline TimeT micros()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch())
                    .count();
    }

    inline TimeT millis()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch())
                    .count();
    }

    inline QString microsToString(qs::TimeT time)
    {
        qs::TimeT t = time/1000;
        int16_t ms = int16_t((t)%1000);
        t /= 1000;
        int16_t s = t % 60;
        t /= 60;
        int16_t m = t % 60;
        t /= 60;
        int16_t h = t % 24;
        return QString::number(h) + ":" + QString::number(m) + ":" +
                QString::number(s) + "." + QString::number(ms);
    }
/*
    template <typename objectT, typename objectProtT = none_t,
              class A = std::allocator<std::pair<objectT, objectProtT>>>
//help: https://stackoverflow.com/questions/7758580/writing-your-own-stl-container/7759622#7759622
    class Tree
    {
    public:
        class iterator : std::forward_iterator_tag
        {
        public:
            struct  node
            {
                int32_t     val;
                Tree        *current;
            };
        private:
            std::vector<node>       pos;
            void                    add(std::vector<node> &vec)
            {
                auto nd = *pos.back();
                if(nd.current.sub_objects.empty())
                {
                    pos.erase(pos.back());
                    pos.back().sub_objects.empty();
                }
            }
        public:
            iterator(const iterator& it)
            {
                pos = it.pos;
            }
            ~iterator()
            {
                pos.clear();
            }
            iterator&   operator=(const iterator& it)
            {
                return iterator(it);
            }
            Tree*       operator*() const
            {
                return pos.empty() ? nullptr : (*pos.back()).current;
            }
            iterator&   operator++()
            {
                auto nd = *pos.back();
                if(nd.current.sub_objects.empty())
                {
                    pos.erase(pos.back());
                    this++;
                }

            }
        };

    private:
        std::vector<Tree*>                  sub_objects;
        std::vector<bool>                   *active;
        Tree                                *prev_obj;
    public:
        objectT                             object;
        objectProtT                         prot;
    };
*/
}



Q_DECLARE_METATYPE(qs::TimeT)

#endif // QSHEADER_H
