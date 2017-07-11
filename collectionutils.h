#pragma once

namespace ams
{

// form all permutations of ab in x
template <class Cont1, class Cont2, class Cont3> void Permute(Cont1& x, const Cont2& a, const Cont3& b)
{
    for (auto p : a)
    {
        for (auto q : b)
        {
            x.push_back(Cont1::value_type(p, q));
        }
    }
}
}

