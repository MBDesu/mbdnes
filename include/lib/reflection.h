#pragma once
#ifndef MODULE_LIB_REFLECTION_H
#define MODULE_LIB_REFLECTION_H

template <typename Tag, typename MemberType> struct PrivateMemberStorage {
  inline static MemberType ptr;
};

template <typename Tag, typename MemberType, MemberType MemberPtr>
struct PrivateAccessor {
  struct Instantiator {
    Instantiator() { PrivateMemberStorage<Tag, MemberType>::ptr = MemberPtr; }
  };
  inline static Instantiator instantiator;
};

#define EXPOSE_PRIVATE_FIELD(Tag, Class, Type, Member)                         \
  struct Tag {};                                                               \
  template struct PrivateAccessor<Tag, Type Class::*, &Class::Member>;         \
  inline auto get_##Member() {                                                 \
    return PrivateMemberStorage<Tag, Type Class::*>::ptr;                      \
  }

#define EXPOSE_PRIVATE_FUNC(Tag, Class, ReturnType, Member, ...)               \
  struct Tag {};                                                               \
  template struct PrivateAccessor<Tag, ReturnType (Class::*)(__VA_ARGS__),     \
                                  &Class::Member>;                             \
  inline auto get_func_##Member() {                                            \
    return PrivateMemberStorage<Tag, ReturnType (Class::*)(__VA_ARGS__)>::ptr; \
  }

#endif // MODULE_LIB_REFLECTION_H
