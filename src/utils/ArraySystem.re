let createEmpty = () => [||];

let flatten = (arr: array('item)) =>
  arr |> Js.Array.reduce((a, b) => Js.Array.concat(b, a), createEmpty());

let getFirst = (arr) =>
  switch (arr |> Array.length) {
  | 0 => ExcepetionHandleSystem.throwMessage({j|getFirst:the array is empty|j})
  | _ => arr[0]
  };