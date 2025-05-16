#include <atomic>
#include <iostream>

template <typename T> class shared_ptr;
template <typename T> class weak_ptr;

template <typename T>
class RefCount{
    
    public:
    RefCount(T *raw_ptr) : _ptr(raw_ptr) {}

    void add_ref(){
        ++_uses;
    }

    void sub_ref(){
        if((--_uses) == 0){
            //释放资源并减少占位的弱引用计数
            //只有弱引用计数也为零的时候才删除控制块
            destroy_resource();
            sub_wref();
        }
    }

    void add_wref(){
        ++_weaks;
    }

    void sub_wref(){
        if((--_weaks) == 0){
            //只有当弱引用计数为零的时候才删除控制块以防止悬空引用
            destroy_this();
        }
    }

    bool try_add_ref(){
        if(_uses == 0)
            return false;
        ++_uses;
        return true;
    }

    void destroy_resource() noexcept{
        delete _ptr;
    }

    void destroy_this() noexcept{
        delete this;
    }

    int32_t use_count(){
        return _uses;
    }

    int32_t weak_count(){
        return _weaks - 1;
    }

    private:
    //_weaks = 1用于占位，_weaks > 1说明有弱指针指向这个资源
    std::atomic_int32_t _uses = 1;
    std::atomic_int32_t _weaks = 1;
    T* _ptr;
};

template <typename T>
class Ptr_base{
    friend class shared_ptr<T>;
    friend class weak_ptr<T>;

    public:
    Ptr_base() = default;
    Ptr_base(const Ptr_base&) = delete;
    Ptr_base& operator=(const Ptr_base&) = delete;

    T* get() const noexcept{
        return _ptr;
    }
    
    auto use_count() const noexcept{
        return _ref ? _ref->use_count() : 0;
    }

    auto weak_count() const noexcept{
        return _ref ? _ref->weak_count() : 0;
    }

    template <typename T2>
    void swap(Ptr_base<T2> &rhs){
        std::swap(_ptr, rhs._ptr);
        std::swap(_ref, rhs._ref);
    }

    private:
    T *_ptr = nullptr;
    RefCount<T> *_ref = nullptr;

    void _init(T *raw_ptr){
        _ptr = raw_ptr;
        _ref = new RefCount(raw_ptr);
    }

    void add_ref() const{
        std::cout << "add " << std::endl;
        if(_ref)
            _ref->add_ref();
    }

    void add_wref() const{
        if(_ref)
            _ref->add_wref();
    }

    void sub_ref() const{
        if(_ref)
            _ref->sub_ref();
    }

    void sub_wref() const{
        if(_ref)
            _ref->sub_wref();
    }

    // 只负责复制内容不考虑引用计数
    template <typename T2>
    void _copy_construct_from(const Ptr_base<T2>& rhs){
        _ptr = rhs._ptr;
        _ref = rhs._ref;
    }

    // 只负责移动内容不考虑引用计数
    template <typename T2>
    void _move_construct_from(Ptr_base<T2> &&rhs) noexcept{
        _copy_construct_from(rhs);
        rhs._ptr = nullptr;
        rhs._ref = nullptr;
    }

    // 先释放原有资源（原有资源的引用计数减一），再增加被移动元素的引用计数，最后复制
    template <typename T2>
    void _copy_ptr_from_shared(const Ptr_base<T2> &rhs){
        sub_ref();
        rhs.add_ref();
        _copy_construct_from(rhs);
    }

    // 先释放原有资源（原有资源的引用计数减一），然后移动元素
    template <typename T2>
    void _move_ptr_from_shared(Ptr_base<T2> &&rhs){
        sub_ref();
        _move_construct_from(std::move(rhs));
    }

    template <typename T2>
    void _copy_weak_ptr_from(const Ptr_base<T2> &rhs){
        sub_wref();
        rhs.add_wref();
        _copy_construct_from(rhs);
    }

    template <typename T2>
    void _move_weak_ptr_from(Ptr_base<T2> &&rhs){
        sub_wref();
        _move_construct_from(std::move(rhs));
    }

    template <typename T2>
    bool _construct_from_weak(const weak_ptr<T2> &rhs) noexcept{
        if(rhs._ref && rhs._ref->try_add_ref()){
            _copy_construct_from(rhs);
            return true;
        }
        return false;
    }

};

template <typename T>
class shared_ptr : public Ptr_base<T>{
    public:
    using Base = Ptr_base<T>;

    shared_ptr() noexcept = default;

    shared_ptr(std::nullptr_t) noexcept {}
    
    explicit shared_ptr(T *raw_ptr){
        Base::_init(raw_ptr);
    }

    //拷贝构造
    shared_ptr(const shared_ptr &rhs){
        Base::_copy_ptr_from_shared(rhs);
    }

    //移动构造
    shared_ptr(shared_ptr &&rhs){
        Base::_move_ptr_from_shared(std::move(rhs));
    }

    //析构函数
    ~shared_ptr(){
        Base::sub_ref();
    }

    //拷贝赋值
    shared_ptr& operator=(const shared_ptr &rhs){
        if(this != &rhs)
            Base::_copy_ptr_from_shared(rhs);
        return *this;
    }

    //移动赋值
    shared_ptr& operator=(shared_ptr &&rhs){
        if(this != &rhs)
            Base::_move_ptr_from_shared(std::move(rhs));
        return *this;
    }

    T& operator*() noexcept{
        return *Base::_ptr;
    }

    const T& operator*() const noexcept{
        return *Base::_ptr;
    }

    T* operator->() noexcept{
        return Base::_ptr;
    }

    const T* operator->() const noexcept{
        return Base::_ptr;
    }

    // 释放资源，变为空指针
    void reset(){
        this->sub_ref();
        Base::_ptr = nullptr;
        Base::_ref = nullptr;
    }
    

};

template <typename T>
class weak_ptr : public Ptr_base<T>{
    public:
    using Base = Ptr_base<T>;

    weak_ptr() = default;
    constexpr weak_ptr(std::nullptr_t) noexcept {};

    weak_ptr(const weak_ptr &rhs){
        Base::_copy_weak_ptr_from(rhs);
    }

    weak_ptr(const shared_ptr<T> &rhs){
        Base::_copy_weak_ptr_from(rhs);
    }

    weak_ptr(weak_ptr &&rhs){
        Base::_move_weak_ptr_from(std::move(rhs));
    }

    weak_ptr& operator=(const weak_ptr &rhs){
        if(this != &rhs)
            Base::_copy_weak_ptr_from(rhs);
        return *this;
    }

    weak_ptr& operator=(weak_ptr &&rhs){
        if(this != &rhs)
            Base::_move_weak_ptr_from(std::move(rhs));
        return *this;
    }

    shared_ptr<T> lock() const{
        shared_ptr<T> ret;
        ret._construct_from_weak(*this);
        return ret;
    }

    ~weak_ptr(){
        Base::sub_wref();
    }

};