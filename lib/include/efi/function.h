#pragma once

template <class TReturn, class... TArgs>
struct Function
{
    [[nodiscard]] virtual TReturn operator()(TArgs... args) const = 0;
};
