This repo contains a sort of "remaking" of various (fully constexpr) features of the C++ std:: namespace, as well as some custom items.
The idea of this project is that I could drop and replace the standard library at any point and use this instead in future C++ projects.
It also provides a way to experiment with ideas I have in C++, and see what things I can create that I think would be useful as a developer.

- String/StringView
- HashMap/HashSet
- Optional
- A few generic standard algorithms
- DynamicArray (std::vector)
- Smart Pointers (includes COW pointer {ImmutableSharedPointer})
- LinkedList
- BufferView
- Others...
- variant namespace, which includes the following:
  - variant::Map   - A HashMap<String, variant::Value>
  - variant::Value - A generic constexpr class that may store any std int type, doubles, floats, Strings, or a DynamicArray of any of those types. Also may store a nested variant::Map object.
  - The variant namespace implements a copy-on-write idiom, where data will be shared so long as you do not attempt to write to it. This makes it more efficient when copying around values and maps.
  - serialization library - Able to serialize to and from buffers with either big or little endian encoding. Serialization is not yet constexpr.