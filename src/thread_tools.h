/**
 * @File   : thread_tools.h
 * @Author : damone (damonexw@gmail.com)
 * @Link   : 
 * @Date   : 11/2/2018, 3:26:56 PM
 */

#ifndef _THREAD_TOOS_
#define _THREAD_TOOS_

#include "platform.h"
#include <list>

namespace ncnn
{
template <typename T>
class SafeOrderList
{
public:
  typedef std::list<T> TContainer;
  typedef typename TContainer::iterator Iterator;

  /**
   * compare function
   * if first <  second return -1
   * if first >  second return 1
   * if first == second return 0
   */
  typedef int (*CompareOperator)(const T &, const T &);

  /**
   * match function
   * if match return true
   * if not match return false
   */
  typedef bool (*MatchOperator)(const T &);

  /**
   * modify function
   */
  typedef void (*ModifyOperator)(T &);

  SafeOrderList(CompareOperator operation) : mutex()
  {
    condition = operation;
  }

  bool empty()
  {
    MutexLockGuard gurad(mutex);
    return container.empty();
  }

  size_t size()
  {
    MutexLockGuard guard(mutex);
    return container.size();
  }

  void insert(const T &value)
  {
    MutexLockGuard guard(mutex);
    Iterator iter = container.begin();
    for (; iter != container.end(); iter++)
    {
      if (condition(value, *iter) > 0)
        break;
    }
    container.insert(iter, value);
  }

  void clear()
  {
    MutexLockGuard guard(mutex);
    container.clear();
  }

  T &front()
  {
    MutexLockGuard guard(mutex);
    return container.front();
  }

  T &back()
  {
    MutexLockGuard guard(mutex);
    return container.back();
  }

  void pop_front()
  {
    MutexLockGuard guard(mutex);
    container.pop_front();
  }

  void pop_back()
  {
    MutexLockGuard guard(mutex);
    container.pop_back();
  }

  void remove_first_match(MatchOperator match)
  {
    MutexLockGuard guard(mutex);
    Iterator iter = container.begin();
    for (; iter != container.end(); iter++)
    {
      if (match(*iter))
      {
        container.erase(iter);
        break;
      }
    }
  }

  template <typename MatchFun>
  void remove_first_match(MatchFun match)
  {
    MutexLockGuard guard(mutex);
    Iterator iter = container.begin();
    for (; iter != container.end(); iter++)
    {
      if (match(*iter))
      {
        container.erase(iter);
        break;
      }
    }
  }

  void remove_all_match(MatchOperator match)
  {
    MutexLockGuard guard(mutex);
    Iterator iter = container.begin();
    for (; iter != container.end();)
    {
      if (match(*iter))
        container.erase(iter++);
      else
        iter++;
    }
  }

  template <typename MatchFun>
  void remove_all_match(MatchFun match)
  {
    MutexLockGuard guard(mutex);
    Iterator iter = container.begin();
    for (; iter != container.end();)
    {
      if (match(*iter))
        container.erase(iter++);
      else
        iter++;
    }
  }

  void for_each(ModifyOperator operation)
  {
    MutexLockGuard guard(mutex);
    Iterator iter = container.begin();
    for (; iter != container.end(); iter++)
    {
      operation(*iter);
    }
  }

  template <typename ModifyFun>
  void for_each(ModifyFun operation)
  {
    MutexLockGuard guard(mutex);
    Iterator iter = container.begin();
    for (; iter != container.end(); iter++)
    {
      operation(*iter);
    }
  }

  void copy_to_list(std::list<T> &dest)
  {
    MutexLockGuard guard(mutex);
    dest.insert(dest.end(), container.begin(), container.end());
  }

private:
  TContainer container;
  CompareOperator condition;
  Mutex mutex;
};

} // namespace ncnn

#endif