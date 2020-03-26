#pragma once

template <class TReturn, class... TArgs>
struct Function
{
    [[nodiscard]] virtual TReturn get(TArgs... args) const = 0;
};
