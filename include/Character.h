#pragma once
#include <stdexcept>


class Characters{
    public:
    enum type{
        JOHN = 0,
        MCDOE = 1,
        ALI = 2,
    };
    constexpr static int toInt(type e){
        switch (e) {
            case JOHN:
                return 0;
            case MCDOE:
                return 1;
            case ALI:
                return 2;
        }
        throw std::runtime_error("invalid enum");
    }
    constexpr static int size(){return 3;};
    constexpr static type fromInt(int i){
        switch (i) {
            case 0:
                return JOHN;
            case 1:
                return MCDOE;
            case 2:
                return ALI;
            default:
                throw std::runtime_error("invalid int");
        }
    }
};