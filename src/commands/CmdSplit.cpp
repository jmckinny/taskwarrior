////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2006 - 2021, Tomas Babej, Paul Beckingham, Federico Hernandez.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// https://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
// cmake.h include header must come first

#include <CmdSplit.h>
#include <Context.h>
#include <Filter.h>
#include <format.h>
#include <main.h>
#include <shared.h>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
CmdSplit::CmdSplit() {
  _keyword = "split";
  _usage = "task <filter> split <mods>";
  _description = "Splits the specified task into multiple subtasks";
  _read_only = false;
  _displays_id = false;
  _needs_confirm = true;
  _needs_gc = false;
  _uses_context = true;
  _accepts_filter = true;
  _accepts_modifications = false;
  _accepts_miscellaneous = true;
  _category = Command::Category::operation;
}

////////////////////////////////////////////////////////////////////////////////
int CmdSplit::execute(std::string&) {
  Filter filter;
  std::vector<Task> filtered;
  filter.subset(filtered);
  if (filtered.size() == 0) {
    Context::getContext().footnote("No tasks specified.");
    return 1;
  }

  if (filtered.size() != 1) {
    std::cout << filtered.size() << std::endl;
    Context::getContext().footnote("Only one task can be split at a time");
    return 1;
  }
  std::vector<std::string> sub_tasks = Context::getContext().cli2.getWords();

  Task to_split(filtered[0]);
  std::string project_name = to_split.get("description");

  std::string question;
  question = format("Split task {1} '{2}' into {3} subtasks?", to_split.identifier(true),
                    to_split.get("description"), sub_tasks.size());

  if (permission(question, filtered.size())) {
    to_split.setStatus(Task::deleted);
    Context::getContext().tdb2.add(to_split);
  }

  for (std::string word : sub_tasks) {
    Task task;
    task.set("description", word);
    task.set("project", project_name);
    Context::getContext().tdb2.add(task);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
