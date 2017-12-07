type stats;

[@bs.module "fs"] external statSync : string => stats = "";

external statsToJsObj : stats => Js.t({..}) = "%identity";

/* let isFilePathExist = (filePath) =>
  filePath
  |> Node.Fs.existsSync
  |> (
    (result) =>
      switch result {
      | false => ExcepetionHandleSystem.throwMessage({j|the $filePath is not exist|j})
      | true => filePath
      }
  ); */