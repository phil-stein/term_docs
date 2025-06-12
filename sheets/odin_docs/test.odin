package term_docs_odin_test

import "core:fmt"

main :: proc()
{
  fmt.println( "hello world" )
  arr_01 := make( [dynamic]int, 10 )
  fmt.println( "cap: ", cap(arr_01) )

  arr_02 : [10]int
  fmt.println( "len: ", len(arr_02) )
}

procedure :: proc( arg0: int, arg1: []f32, arg2: rawptr ) -> ( res0: int, res1: f64 )
{
  if arg2 != nil
  {
    index : int
    index = arg0 
    return arg0, f64(arg1[index])
  }
  else { return arg0, 0.0 }
}

