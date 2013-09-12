// Copyright (c) 2013, Huang-Ming Huang,  Object Computing, Inc.
// All rights reserved.
//
// This file is part of mFAST.
//
//     mFAST is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Lesser General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     mFAST is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU Lesser General Public License
//     along with mFast.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef NESTED_MESSAGE_REF_H_X549MWYP
#define NESTED_MESSAGE_REF_H_X549MWYP

#include "mfast/field_instruction.h"
#include "mfast/field_ref.h"
#include "mfast/message_ref.h"

namespace mfast
{

class nested_message_cref
  : public field_cref
{
  public:
    typedef const templateref_instruction* instruction_cptr;
    typedef boost::false_type is_mutable;

    nested_message_cref(const value_storage* storage,
                        instruction_cptr     inst)
      : field_cref(storage, inst)
    {
    }

    explicit nested_message_cref(const field_cref& other)
      : field_cref(other)
    {
    }

    const templateref_instruction* instruction() const
    {
      return static_cast<const templateref_instruction*> (instruction_);
    }

    bool is_static() const
    {
      return instruction()->is_static();
    }
    
    message_cref target() const 
    {
      return message_cref(*this);
    }
    
    const template_instruction* target_instruction() const
    {
      return storage()->of_templateref.of_instruction.instruction_;
    }
    
    operator aggregate_cref() const
    {
      return aggregate_cref(storage()->of_group.content_, target_instruction());
    }
    
    template <typename FieldAccessor>
    void accept_accessor(FieldAccessor&) const;

};


class nested_message_mref
  : public make_field_mref<nested_message_cref>
{
  public:
    typedef boost::true_type is_mutable;
    typedef mfast::allocator allocator_type;

    nested_message_mref(allocator_type*                alloc,
                        value_storage*                 storage,
                        const templateref_instruction* inst)
      : make_field_mref<nested_message_cref>(alloc, storage, inst)
    {
    }

    nested_message_mref(const field_mref_base& other)
      : make_field_mref<nested_message_cref>(other)
    {
    }
    
    message_mref target() const 
    {
      return message_mref(alloc_, this->storage(), this->target_instruction());
    }
    
    operator aggregate_mref() const
    {
      return aggregate_mref(alloc_, storage()->of_group.content_, this->target_instruction());
    }

    template <typename FieldMutator>
    void accept_mutator(FieldMutator&) const;
    
    template <typename MESSAGE>
    typename MESSAGE::mref_type as() const
    {
      typedef typename MESSAGE::mref_type mref_type;
      set_target_instruction(&MESSAGE::the_instruction, true);
      return mref_type(alloc_, this->storage(), &MESSAGE::the_instruction);
    }

    message_mref rebind(const template_instruction* inst) const
    {
      set_target_instruction(inst, true);
      return message_mref(alloc_, this->storage()->of_templateref.content_, inst);
    }

    void set_target_instruction(const template_instruction* inst, bool construct_subfields = true) const
    {
      const template_instruction*& target_inst = this->storage()->of_templateref.of_instruction.instruction_;
      
      assert(!is_static() || target_inst == 0);

      if (inst == target_inst)
        return;

      if (target_inst) {
        target_inst->destruct_value(*this->storage(), alloc_);
      }
      inst->construct_value(*this->storage(), 0, alloc_, construct_subfields);
      target_inst = inst;
    }
};


}


#endif /* end of include guard: NESTED_MESSAGE_REF_H_X549MWYP */
