// Generado por tools/gen_cxx.py: imports C++ del juego -> libstdc++ de VitaSDK.
// No editar a mano (volver a correr gen_cxx.py con el report.txt de ios_prep.py).
#include <string.h>

#include "imports.h"

extern char cxx_0 __asm__("_ZNKSs12find_last_ofEPKcjj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::find_last_of(char const*, unsigned long, unsigned long) const
extern char cxx_1 __asm__("_ZNKSs4findEPKcj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::find(char const*, unsigned long) const
extern char cxx_2 __asm__("_ZNKSs4findEPKcjj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::find(char const*, unsigned long, unsigned long) const
extern char cxx_3 __asm__("_ZNKSs4findEcj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::find(char, unsigned long) const
extern char cxx_4 __asm__("_ZNKSs5rfindEcj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::rfind(char, unsigned long) const
extern char cxx_5 __asm__("_ZNKSs6substrEjj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::substr(unsigned long, unsigned long) const
extern char cxx_6 __asm__("_ZNKSs7compareEPKc"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::compare(char const*) const
extern char cxx_7 __asm__("_ZNKSs7compareERKSs"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::compare(std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) const
extern char cxx_8 __asm__("_ZNKSs7compareEjjRKSs"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::compare(unsigned long, unsigned long, std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) const
extern char cxx_9 __asm__("_ZNKSt15basic_stringbufIcSt11char_traitsIcESaIcEE3strEv"); // std::basic_stringbuf<char, std::char_traits<char>, std::allocator<char> >::str() const
extern char cxx_10 __asm__("_ZNKSt9basic_iosIcSt11char_traitsIcEE5widenEc"); // std::basic_ios<char, std::char_traits<char> >::widen(char) const
extern char cxx_11 __asm__("_ZNSi4peekEv"); // std::basic_istream<char, std::char_traits<char> >::peek()
extern char cxx_12 __asm__("_ZNSi4readEPci"); // std::basic_istream<char, std::char_traits<char> >::read(char*, int)
extern char cxx_13 __asm__("_ZNSi5seekgExSt12_Ios_Seekdir"); // std::basic_istream<char, std::char_traits<char> >::seekg(long long, std::_Ios_Seekdir)
extern char cxx_14 __asm__("_ZNSi5tellgEv"); // std::basic_istream<char, std::char_traits<char> >::tellg()
extern char cxx_15 __asm__("_ZNSo9_M_insertIdEERSoT_"); // std::basic_ostream<char, std::char_traits<char> >& std::basic_ostream<char, std::char_traits<char> >::_M_insert<double>(double)
extern char cxx_16 __asm__("_ZNSo9_M_insertImEERSoT_"); // std::basic_ostream<char, std::char_traits<char> >& std::basic_ostream<char, std::char_traits<char> >::_M_insert<unsigned long>(unsigned long)
extern char cxx_17 __asm__("_ZNSolsEi"); // std::basic_ostream<char, std::char_traits<char> >::operator<<(int)
extern char cxx_18 __asm__("_ZNSs12_M_leak_hardEv"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_leak_hard()
extern char cxx_19 __asm__("_ZNSs2atEj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::at(unsigned long)
extern char cxx_20 __asm__("_ZNSs4_Rep10_M_destroyERKSaIcE"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::_Rep::_M_destroy(std::allocator<char> const&)
extern char cxx_21 __asm__("_ZNSs4_Rep20_S_empty_rep_storageE"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::_Rep::_S_empty_rep_storage
extern char cxx_22 __asm__("_ZNSs4_Rep9_S_createEjjRKSaIcE"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::_Rep::_S_create(unsigned long, unsigned long, std::allocator<char> const&)
extern char cxx_23 __asm__("_ZNSs6appendEPKcj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::append(char const*, unsigned long)
extern char cxx_24 __asm__("_ZNSs6appendERKSs"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::append(std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
extern char cxx_25 __asm__("_ZNSs6appendEjc"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::append(unsigned long, char)
extern char cxx_26 __asm__("_ZNSs6assignEPKcj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::assign(char const*, unsigned long)
extern char cxx_27 __asm__("_ZNSs6assignERKSs"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::assign(std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
extern char cxx_28 __asm__("_ZNSs7replaceEjjPKcj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::replace(unsigned long, unsigned long, char const*, unsigned long)
extern char cxx_29 __asm__("_ZNSs7reserveEj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::reserve(unsigned long)
extern char cxx_30 __asm__("_ZNSsC1EPKcRKSaIcE"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(char const*, std::allocator<char> const&)
extern char cxx_31 __asm__("_ZNSsC1ERKSs"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
extern char cxx_32 __asm__("_ZNSsC1ERKSsjj"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, unsigned long, unsigned long)
extern char cxx_33 __asm__("_ZNSsC1EjcRKSaIcE"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(unsigned long, char, std::allocator<char> const&)
extern char cxx_34 __asm__("_ZNSsD1Ev"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::~basic_string()
extern char cxx_35 __asm__("_ZNSsD2Ev"); // std::basic_string<char, std::char_traits<char>, std::allocator<char> >::~basic_string()
extern char cxx_36 __asm__("_ZNSt12__basic_fileIcED1Ev"); // std::__basic_file<char>::~__basic_file()
extern char cxx_37 __asm__("_ZNSt13basic_filebufIcSt11char_traitsIcEE4openEPKcSt13_Ios_Openmode"); // std::basic_filebuf<char, std::char_traits<char> >::open(char const*, std::_Ios_Openmode)
extern char cxx_38 __asm__("_ZNSt13basic_filebufIcSt11char_traitsIcEE5closeEv"); // std::basic_filebuf<char, std::char_traits<char> >::close()
extern char cxx_39 __asm__("_ZNSt13basic_filebufIcSt11char_traitsIcEEC1Ev"); // std::basic_filebuf<char, std::char_traits<char> >::basic_filebuf()
extern char cxx_40 __asm__("_ZNSt15_List_node_base4hookEPS_"); // std::_List_node_base::hook(std::_List_node_base*)
extern char cxx_41 __asm__("_ZNSt15_List_node_base4swapERS_S0_"); // std::_List_node_base::swap(std::_List_node_base&, std::_List_node_base&)
extern char cxx_42 __asm__("_ZNSt15_List_node_base6unhookEv"); // std::_List_node_base::unhook()
extern char cxx_43 __asm__("_ZNSt15_List_node_base8transferEPS_S0_"); // std::_List_node_base::transfer(std::_List_node_base*, std::_List_node_base*)
extern char cxx_44 __asm__("_ZNSt15basic_stringbufIcSt11char_traitsIcESaIcEE7_M_syncEPcjj"); // std::basic_stringbuf<char, std::char_traits<char>, std::allocator<char> >::_M_sync(char*, unsigned long, unsigned long)
extern char cxx_45 __asm__("_ZNSt6localeC1Ev"); // std::locale::locale()
extern char cxx_46 __asm__("_ZNSt6localeD1Ev"); // std::locale::~locale()
extern char cxx_47 __asm__("_ZNSt8ios_base4InitC1Ev"); // std::ios_base::Init::Init()
extern char cxx_48 __asm__("_ZNSt8ios_base4InitD1Ev"); // std::ios_base::Init::~Init()
extern char cxx_49 __asm__("_ZNSt8ios_baseC2Ev"); // std::ios_base::ios_base()
extern char cxx_50 __asm__("_ZNSt8ios_baseD2Ev"); // std::ios_base::~ios_base()
extern char cxx_51 __asm__("_ZNSt9bad_allocD1Ev"); // std::bad_alloc::~bad_alloc()
extern char cxx_52 __asm__("_ZNSt9basic_iosIcSt11char_traitsIcEE4initEPSt15basic_streambufIcS1_E"); // std::basic_ios<char, std::char_traits<char> >::init(std::basic_streambuf<char, std::char_traits<char> >*)
extern char cxx_53 __asm__("_ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate"); // std::basic_ios<char, std::char_traits<char> >::clear(std::_Ios_Iostate)
extern char cxx_54 __asm__("_ZNSt9exceptionD2Ev"); // std::exception::~exception()
extern char cxx_55 __asm__("_ZSt15set_new_handlerPFvvE"); // std::set_new_handler(void (*)())
extern char cxx_56 __asm__("_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i"); // std::basic_ostream<char, std::char_traits<char> >& std::__ostream_insert<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*, int)
extern char cxx_57 __asm__("_ZSt17__throw_bad_allocv"); // std::__throw_bad_alloc()
extern char cxx_58 __asm__("_ZSt18_Rb_tree_decrementPSt18_Rb_tree_node_base"); // std::_Rb_tree_decrement(std::_Rb_tree_node_base*)
extern char cxx_59 __asm__("_ZSt18_Rb_tree_incrementPKSt18_Rb_tree_node_base"); // std::_Rb_tree_increment(std::_Rb_tree_node_base const*)
extern char cxx_60 __asm__("_ZSt18_Rb_tree_incrementPSt18_Rb_tree_node_base"); // std::_Rb_tree_increment(std::_Rb_tree_node_base*)
extern char cxx_61 __asm__("_ZSt18uncaught_exceptionv"); // std::uncaught_exception()
extern char cxx_62 __asm__("_ZSt19__throw_logic_errorPKc"); // std::__throw_logic_error(char const*)
extern char cxx_63 __asm__("_ZSt20__throw_length_errorPKc"); // std::__throw_length_error(char const*)
extern char cxx_64 __asm__("_ZSt20__throw_out_of_rangePKc"); // std::__throw_out_of_range(char const*)
extern char cxx_65 __asm__("_ZSt28_Rb_tree_rebalance_for_erasePSt18_Rb_tree_node_baseRS_"); // std::_Rb_tree_rebalance_for_erase(std::_Rb_tree_node_base*, std::_Rb_tree_node_base&)
extern char cxx_66 __asm__("_ZSt29_Rb_tree_insert_and_rebalancebPSt18_Rb_tree_node_baseS0_RS_"); // std::_Rb_tree_insert_and_rebalance(bool, std::_Rb_tree_node_base*, std::_Rb_tree_node_base*, std::_Rb_tree_node_base&)
extern char cxx_67 __asm__("_ZSt7nothrow"); // std::nothrow
extern char cxx_68 __asm__("_ZSt9terminatev"); // std::terminate()
extern char cxx_69 __asm__("_ZTIPKc"); // typeinfo for char const*
extern char cxx_70 __asm__("_ZTIPKh"); // typeinfo for unsigned char const*
extern char cxx_71 __asm__("_ZTIPKi"); // typeinfo for int const*
extern char cxx_72 __asm__("_ZTIPKw"); // typeinfo for wchar_t const*
extern char cxx_73 __asm__("_ZTISt9bad_alloc"); // typeinfo for std::bad_alloc
extern char cxx_74 __asm__("_ZTIb"); // typeinfo for bool
extern char cxx_75 __asm__("_ZTIh"); // typeinfo for unsigned char
extern char cxx_76 __asm__("_ZTIi"); // typeinfo for int
extern char cxx_77 __asm__("_ZTTSt14basic_ifstreamIcSt11char_traitsIcEE"); // VTT for std::basic_ifstream<char, std::char_traits<char> >
extern char cxx_78 __asm__("_ZTTSt19basic_ostringstreamIcSt11char_traitsIcESaIcEE"); // VTT for std::basic_ostringstream<char, std::char_traits<char>, std::allocator<char> >
extern char cxx_79 __asm__("_ZTVSt13basic_filebufIcSt11char_traitsIcEE"); // vtable for std::basic_filebuf<char, std::char_traits<char> >
extern char cxx_80 __asm__("_ZTVSt14basic_ifstreamIcSt11char_traitsIcEE"); // vtable for std::basic_ifstream<char, std::char_traits<char> >
extern char cxx_81 __asm__("_ZTVSt15basic_streambufIcSt11char_traitsIcEE"); // vtable for std::basic_streambuf<char, std::char_traits<char> >
extern char cxx_82 __asm__("_ZTVSt15basic_stringbufIcSt11char_traitsIcESaIcEE"); // vtable for std::basic_stringbuf<char, std::char_traits<char>, std::allocator<char> >
extern char cxx_83 __asm__("_ZTVSt19basic_ostringstreamIcSt11char_traitsIcESaIcEE"); // vtable for std::basic_ostringstream<char, std::char_traits<char>, std::allocator<char> >
extern char cxx_84 __asm__("_ZTVSt9bad_alloc"); // vtable for std::bad_alloc
extern char cxx_85 __asm__("_ZTVSt9basic_iosIcSt11char_traitsIcEE"); // vtable for std::basic_ios<char, std::char_traits<char> >
extern char cxx_86 __asm__("_ZTVSt9exception"); // vtable for std::exception
extern char cxx_87 __asm__("_ZdaPv"); // operator delete[](void*)
extern char cxx_88 __asm__("_ZdlPv"); // operator delete(void*)
extern char cxx_89 __asm__("_Znaj"); // operator new[](unsigned long)
extern char cxx_90 __asm__("_ZnajRKSt9nothrow_t"); // operator new[](unsigned long, std::nothrow_t const&)
extern char cxx_91 __asm__("_Znwj"); // operator new(unsigned long)

const import_entry cxx_imports[] = {
    {"__ZNKSs12find_last_ofEPKcmm", (uintptr_t)&cxx_0, 0},
    {"__ZNKSs4findEPKcm", (uintptr_t)&cxx_1, 0},
    {"__ZNKSs4findEPKcmm", (uintptr_t)&cxx_2, 0},
    {"__ZNKSs4findEcm", (uintptr_t)&cxx_3, 0},
    {"__ZNKSs5rfindEcm", (uintptr_t)&cxx_4, 0},
    {"__ZNKSs6substrEmm", (uintptr_t)&cxx_5, 0},
    {"__ZNKSs7compareEPKc", (uintptr_t)&cxx_6, 0},
    {"__ZNKSs7compareERKSs", (uintptr_t)&cxx_7, 0},
    {"__ZNKSs7compareEmmRKSs", (uintptr_t)&cxx_8, 0},
    {"__ZNKSt15basic_stringbufIcSt11char_traitsIcESaIcEE3strEv", (uintptr_t)&cxx_9, 0},
    {"__ZNKSt9basic_iosIcSt11char_traitsIcEE5widenEc", (uintptr_t)&cxx_10, 0},
    {"__ZNSi4peekEv", (uintptr_t)&cxx_11, 0},
    {"__ZNSi4readEPci", (uintptr_t)&cxx_12, 0},
    {"__ZNSi5seekgExSt12_Ios_Seekdir", (uintptr_t)&cxx_13, 0},
    {"__ZNSi5tellgEv", (uintptr_t)&cxx_14, 0},
    {"__ZNSo9_M_insertIdEERSoT_", (uintptr_t)&cxx_15, 0},
    {"__ZNSo9_M_insertImEERSoT_", (uintptr_t)&cxx_16, 0},
    {"__ZNSolsEi", (uintptr_t)&cxx_17, 0},
    {"__ZNSs12_M_leak_hardEv", (uintptr_t)&cxx_18, 0},
    {"__ZNSs2atEm", (uintptr_t)&cxx_19, 0},
    {"__ZNSs4_Rep10_M_destroyERKSaIcE", (uintptr_t)&cxx_20, 0},
    {"__ZNSs4_Rep20_S_empty_rep_storageE", (uintptr_t)&cxx_21, 1},
    {"__ZNSs4_Rep9_S_createEmmRKSaIcE", (uintptr_t)&cxx_22, 0},
    {"__ZNSs6appendEPKcm", (uintptr_t)&cxx_23, 0},
    {"__ZNSs6appendERKSs", (uintptr_t)&cxx_24, 0},
    {"__ZNSs6appendEmc", (uintptr_t)&cxx_25, 0},
    {"__ZNSs6assignEPKcm", (uintptr_t)&cxx_26, 0},
    {"__ZNSs6assignERKSs", (uintptr_t)&cxx_27, 0},
    {"__ZNSs7replaceEmmPKcm", (uintptr_t)&cxx_28, 0},
    {"__ZNSs7reserveEm", (uintptr_t)&cxx_29, 0},
    {"__ZNSsC1EPKcRKSaIcE", (uintptr_t)&cxx_30, 0},
    {"__ZNSsC1ERKSs", (uintptr_t)&cxx_31, 0},
    {"__ZNSsC1ERKSsmm", (uintptr_t)&cxx_32, 0},
    {"__ZNSsC1EmcRKSaIcE", (uintptr_t)&cxx_33, 0},
    {"__ZNSsD1Ev", (uintptr_t)&cxx_34, 0},
    {"__ZNSsD2Ev", (uintptr_t)&cxx_35, 0},
    {"__ZNSt12__basic_fileIcED1Ev", (uintptr_t)&cxx_36, 0},
    {"__ZNSt13basic_filebufIcSt11char_traitsIcEE4openEPKcSt13_Ios_Openmode", (uintptr_t)&cxx_37, 0},
    {"__ZNSt13basic_filebufIcSt11char_traitsIcEE5closeEv", (uintptr_t)&cxx_38, 0},
    {"__ZNSt13basic_filebufIcSt11char_traitsIcEEC1Ev", (uintptr_t)&cxx_39, 0},
    {"__ZNSt15_List_node_base4hookEPS_", (uintptr_t)&cxx_40, 0},
    {"__ZNSt15_List_node_base4swapERS_S0_", (uintptr_t)&cxx_41, 0},
    {"__ZNSt15_List_node_base6unhookEv", (uintptr_t)&cxx_42, 0},
    {"__ZNSt15_List_node_base8transferEPS_S0_", (uintptr_t)&cxx_43, 0},
    {"__ZNSt15basic_stringbufIcSt11char_traitsIcESaIcEE7_M_syncEPcmm", (uintptr_t)&cxx_44, 0},
    {"__ZNSt6localeC1Ev", (uintptr_t)&cxx_45, 0},
    {"__ZNSt6localeD1Ev", (uintptr_t)&cxx_46, 0},
    {"__ZNSt8ios_base4InitC1Ev", (uintptr_t)&cxx_47, 0},
    {"__ZNSt8ios_base4InitD1Ev", (uintptr_t)&cxx_48, 0},
    {"__ZNSt8ios_baseC2Ev", (uintptr_t)&cxx_49, 0},
    {"__ZNSt8ios_baseD2Ev", (uintptr_t)&cxx_50, 0},
    {"__ZNSt9bad_allocD1Ev", (uintptr_t)&cxx_51, 0},
    {"__ZNSt9basic_iosIcSt11char_traitsIcEE4initEPSt15basic_streambufIcS1_E", (uintptr_t)&cxx_52, 0},
    {"__ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate", (uintptr_t)&cxx_53, 0},
    {"__ZNSt9exceptionD2Ev", (uintptr_t)&cxx_54, 0},
    {"__ZSt15set_new_handlerPFvvE", (uintptr_t)&cxx_55, 0},
    {"__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i", (uintptr_t)&cxx_56, 0},
    {"__ZSt17__throw_bad_allocv", (uintptr_t)&cxx_57, 0},
    {"__ZSt18_Rb_tree_decrementPSt18_Rb_tree_node_base", (uintptr_t)&cxx_58, 0},
    {"__ZSt18_Rb_tree_incrementPKSt18_Rb_tree_node_base", (uintptr_t)&cxx_59, 0},
    {"__ZSt18_Rb_tree_incrementPSt18_Rb_tree_node_base", (uintptr_t)&cxx_60, 0},
    {"__ZSt18uncaught_exceptionv", (uintptr_t)&cxx_61, 0},
    {"__ZSt19__throw_logic_errorPKc", (uintptr_t)&cxx_62, 0},
    {"__ZSt20__throw_length_errorPKc", (uintptr_t)&cxx_63, 0},
    {"__ZSt20__throw_out_of_rangePKc", (uintptr_t)&cxx_64, 0},
    {"__ZSt28_Rb_tree_rebalance_for_erasePSt18_Rb_tree_node_baseRS_", (uintptr_t)&cxx_65, 0},
    {"__ZSt29_Rb_tree_insert_and_rebalancebPSt18_Rb_tree_node_baseS0_RS_", (uintptr_t)&cxx_66, 0},
    {"__ZSt7nothrow", (uintptr_t)&cxx_67, 1},
    {"__ZSt9terminatev", (uintptr_t)&cxx_68, 0},
    {"__ZTIPKc", (uintptr_t)&cxx_69, 1},
    {"__ZTIPKh", (uintptr_t)&cxx_70, 1},
    {"__ZTIPKi", (uintptr_t)&cxx_71, 1},
    {"__ZTIPKw", (uintptr_t)&cxx_72, 1},
    {"__ZTISt9bad_alloc", (uintptr_t)&cxx_73, 1},
    {"__ZTIb", (uintptr_t)&cxx_74, 1},
    {"__ZTIh", (uintptr_t)&cxx_75, 1},
    {"__ZTIi", (uintptr_t)&cxx_76, 1},
    {"__ZTTSt14basic_ifstreamIcSt11char_traitsIcEE", (uintptr_t)&cxx_77, 1},
    {"__ZTTSt19basic_ostringstreamIcSt11char_traitsIcESaIcEE", (uintptr_t)&cxx_78, 1},
    {"__ZTVSt13basic_filebufIcSt11char_traitsIcEE", (uintptr_t)&cxx_79, 1},
    {"__ZTVSt14basic_ifstreamIcSt11char_traitsIcEE", (uintptr_t)&cxx_80, 1},
    {"__ZTVSt15basic_streambufIcSt11char_traitsIcEE", (uintptr_t)&cxx_81, 1},
    {"__ZTVSt15basic_stringbufIcSt11char_traitsIcESaIcEE", (uintptr_t)&cxx_82, 1},
    {"__ZTVSt19basic_ostringstreamIcSt11char_traitsIcESaIcEE", (uintptr_t)&cxx_83, 1},
    {"__ZTVSt9bad_alloc", (uintptr_t)&cxx_84, 1},
    {"__ZTVSt9basic_iosIcSt11char_traitsIcEE", (uintptr_t)&cxx_85, 1},
    {"__ZTVSt9exception", (uintptr_t)&cxx_86, 1},
    {"__ZdaPv", (uintptr_t)&cxx_87, 0},
    {"__ZdlPv", (uintptr_t)&cxx_88, 0},
    {"__Znam", (uintptr_t)&cxx_89, 0},
    {"__ZnamRKSt9nothrow_t", (uintptr_t)&cxx_90, 0},
    {"__Znwm", (uintptr_t)&cxx_91, 0},
};
TABLE_END(cxx)

// Lo que no tiene equivalente queda en el log como "unimpl".
const import_entry *cxx_import(const char *name) { (void)name; return NULL; }
// FALTA __ZNSi5seekgESt4fposI11__mbstate_tE  (std::basic_istream<char, std::char_traits<char> >::seekg(std::fpos<__mbstate_t>))
