#include <utility>
#include <cstddef>
#include <algorithm>
//Adapted from https://github.com/VinGarcia/Simple-Iterator-Template

template<class T> struct search_result_ptr_pair;

// T - The content type
// S - The state keeping structure
template <typename T, template <typename> class S=search_result_ptr_pair>
class search_results 
{
public:
  typedef std::ptrdiff_t difference_type;
  typedef size_t size_type; //
  typedef T  value_type;
  typedef T* pointer;
  typedef T& reference;
  typedef const T* const_pointer;
  typedef const T& const_reference;

  struct const_iterator;
  
  struct iterator {
  private:
    iterator(search_results* ref) : ref(ref) {}
  
    search_results* ref;
    S<T> state;
  public:
    // Note: Instances build with this constructor should
    // be used only after copy-assigning from other iterator!
    iterator() {}
  
  public:
    T& operator*()  { return  state.get(ref); }
    T* operator->() { return &state.get(ref); }
    iterator& operator++() { state.next(ref); return *this; }
    iterator& operator--() { state.prev(ref); return *this; }
    iterator operator++(int) { iterator temp(*this); state.next(ref); return temp; }
    iterator operator--(int) { iterator temp(*this); state.prev(ref); return temp; }
    bool operator!=(const iterator& other) const {
      return ref != other.ref || state.cmp(other.state);
    }
    bool operator==(const iterator& other) const {
      return !operator!=(other);
    }
  
    static iterator begin(search_results* ref) {
      iterator it(ref);
      it.state.begin(ref);
      return it;
    }
    static iterator end(search_results* ref) {
      iterator it(ref);
      it.state.end(ref);
      return it;
    }
  
    friend struct const_iterator;
  
    // Comparisons between const and normal iterators:
    bool operator!=(const const_iterator& other) const {
      return ref != other.ref || cmd(other.state);
    }
    bool operator==(const const_iterator& other) const {
      return !operator!=(other);
    }
  
  };
  struct const_iterator {
  private:
    const_iterator(const search_results* ref) : ref(ref) {}
    const search_results* ref;
    S<T> state;
  
  public:
    // Note: Instances build with this constructor should
    // be used only after copy-assigning from other iterator!
    const_iterator() {}
  
    // To make possible copy-construct non-const iterators:
    const_iterator(const iterator& other) : ref(other.ref) {
      state = other.state;
    }
    
  public:
    const T& operator*()  { return  state.get(ref); }
    const T* operator->() { return &state.get(ref); }
    const_iterator& operator++() { state.next(ref); return *this; }
    const_iterator& operator--() { state.prev(ref); return *this; }
    const_iterator operator++(int) { const_iterator temp(*this); state.next(ref); return temp; }
    const_iterator operator--(int) { const_iterator temp(*this); state.prev(ref); return temp; }
    bool operator!=(const const_iterator& other) const {
      return ref != other.ref || state.cmp(other.state);
    }
    bool operator==(const const_iterator& other) const {
      return !operator!=(other);
    }
    const_iterator& operator=(const iterator& other) {
      ref = other.ref;
      state = other.state;
      return *this;
    }
    static const_iterator begin(const search_results* ref) {
      const_iterator it(ref);
      it.state.begin(ref);
      return it;
    }
    static const_iterator end(const search_results* ref) {
      const_iterator it(ref);
      it.state.end(ref);
      return it;
    }
  
    friend struct iterator;
  
    // Comparisons between const and normal iterators:
    bool operator!=(const iterator& other) const {
      return ref != other.ref || cmp(other.state);
    }
    bool operator==(const iterator& other) const {
      return !operator!=(other);
    }
  };
  
  search_results(const size_t results_max) : 
    head_(new typename S<T>::subresults_type[std::max(S<T>::subresults_max(results_max),(size_t)1)]) {reset();}
  search_results(const search_results& __rhs)            = delete;
  search_results(search_results&& __rhs) noexcept        = delete;
  search_results& operator=(const search_results& __rhs) = delete;
  search_results& operator=(search_results&& __rhs)      = delete;
  ~search_results() {delete[] head_;}
    
 
  void push_interval(const std::pair<T*,T*> &interval) {
    size_+=S<T>::push_interval(tail_,interval);
  }
  void reset() {
    size_=0;
    *head_=S<T>::make_empty_subresult();
    tail_=head_;
  }
  size_t size() const { return size_; }
  bool empty()  const { return size() == 0; }
  
  iterator begin() { return iterator::begin(this); }
  iterator end()   { return iterator::end(this); }
  const_iterator begin() const { return const_iterator::begin(this); }
  const_iterator end()   const { return const_iterator::end(this); }
  const_iterator cbegin() const { return const_iterator::begin(this); }
  const_iterator cend()   const { return const_iterator::end(this); }
  
  friend struct S<T>;
private:
  typename S<T>::subresults_type* head_;
  typename S<T>::subresults_type* tail_;
  size_t size_;
};


template<typename T>
struct search_result_ptr_pair {
  typedef std::pair<T*,T*> subresults_type;
  static subresults_type make_empty_subresult() {return std::make_pair(nullptr,nullptr);} 
  static inline size_t subresults_max(const size_t results_max) {return (results_max+1)/2;};

  //undefined behavior for tail->first > interval.first
  static inline size_t push_interval(subresults_type *&tail_ ,const std::pair<T*,T*> &interval) {
    if (interval.first >= interval.second ) return 0;
    if (tail_->second > tail_->first) {
      if (tail_->second<interval.first) ++tail_;
      else {
        auto temp=tail_->second;
        tail_->second=std::max(tail_->second,interval.second);
        return std::distance(temp,tail_->second);
      }
    }
    *tail_=interval;
    return std::distance(tail_->first,tail_->second);
  }
  
  subresults_type* pos;
  T* subpos;
  
  inline bool cmp (const search_result_ptr_pair<T>& s) const { return (pos != s.pos || subpos != s.subpos); }
  
  template <class C> inline void next (const C* ref) { if ((++subpos)==pos->second) subpos=(++pos)->first;}
  template <class C> inline void prev (const C* ref) { if ((subpos--)==pos->first ) subpos=(--pos)->second-1;}
  template <class C> inline void begin   (const C* ref) {pos = ref->head_; subpos=pos->first;}
  template <class C> inline void end     (const C* ref) {pos = ref->tail_+(ref->size() > 0); subpos=pos->first;};
  template <class C> inline const T& get (const C* ref) { return *subpos; }
  template <class C> inline       T& get       (C* ref) { return *subpos; }
};
