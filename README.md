This yaml reader perform type checking and config validation during static allocation.
Therefore, any invalid type cast of usage of config will be thrown at the very beginning of the program even before the program reaching that point.

All used variables will be statically allocated, and they are parsed at allocatin time. Therefore, any retrieval of config is merely returning a const reference to the stack allocated data.

One down size is the need to use macro for introducing constexpr string type.
