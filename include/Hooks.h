#pragma once
namespace Hooks {

    void Install();

    struct ScrollSpellTypeHook {
        static inline REL::Relocation<RE::MagicSystem::SpellType(RE::ScrollItem*)> originalFunction;
        static RE::MagicSystem::SpellType GetSpellType(RE::ScrollItem* ref);
        static void Install();
    };

    // class GetActorValueForCost {
    // public:
    //     static void Install();

    // private:
    //     static RE::ActorValue thunk(RE::MagicItem* magicItem, bool rightHand);
    //     static inline REL::Relocation<decltype(thunk)> originalFunction;
    // };
}