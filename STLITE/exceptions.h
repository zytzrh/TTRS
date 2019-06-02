
//
// Created by yezhuoyang on 2019-05-13.
//

#ifndef TICKET_OFFICE_EXCEPTION_HPP
#define TICKET_OFFICE_EXCEPTION_HPP
#include <cstddef>
#include <cstring>
#include <string>


    class exception {
    protected:
        const std::string variant = "";
        std::string detail = "";
    public:
        exception() {}
        exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
        virtual std::string what() {
            return variant + " " + detail;
        }
    };

    class wrong_operation : public exception {
        /* __________________________ */
    };


    class runtime_error : public exception {
        /* __________________________ */
    };

    class invalid_iterator : public exception {
        /* __________________________ */
    };

    class container_is_empty : public exception {
        /* __________________________ */
    };

    class not_found : public exception {

    };

    class index_out_of_bound : public exception {

    };

    class invalid_format : public exception {

    };

    class unallocated_space : public exception {

    };






#endif //TICKET_OFFICE_EXCEPTION_HPP

