open FsExtend;

let _removeRedundancePath = (path) =>
  path |> Js.String.slice(~from=3, ~to_=path |> Js.String.length);

let _changeImportCss = (relativePath, fileContent) => {
  let importCssRe = [%re {|/^importCss\s*\((\"|\')/g|}];
  let fileNameRe = [%re {|/[\"|\'](.*?)[\"|\']/g|}];
  let hasChangeFile = ref(false);
  fileContent
  |> Js.String.split("\n")
  |> Js.Array.reduce(
       (resultLine, line) =>
         switch (importCssRe |> Js.Re.test(line)) {
         | false => resultLine ++ line ++ "\n"
         | true =>
           hasChangeFile := true;
           switch (fileNameRe |> Js.Re.exec(line)) {
           | None => resultLine
           | Some(res_) =>
             let fileNameResult = Js.Re.matches(res_);
             let index = fileNameResult[0] |> Js.String.indexOf("/");
             fileNameResult[0]
             |> Js.String.slice(~from=index + 1, ~to_=(-1))
             |> (
               (fileName) => resultLine ++ "import '" ++ relativePath ++ "/" ++ fileName ++ "';\n"
             )
           }
         },
       ""
     )
  |> ((resultFile) => (hasChangeFile, resultFile))
};

let _getRelativePath = (removePathArray, filePath) =>
  filePath
  |> Js.String.split("/")
  |> Js.Array.filter((pathSegment) => ! (removePathArray |> Js.Array.includes(pathSegment)))
  |> Js.Array.slice(~start=0, ~end_=(-1))
  |> Js.Array.joinWith("/")
  |> PathExtend.relative(filePath)
  |> _removeRedundancePath;

let _parseSystem = (removePathArray, filePath) => {
  let relativePath = filePath |> _getRelativePath(removePathArray);
  Node.Fs.readFileSync(filePath, `utf8)
  |> _changeImportCss(relativePath)
  |> (
    ((hasChangeFile, resultFile)) =>
      if (hasChangeFile^) {
        Node.Fs.writeFileSync(~filename=filePath, ~text=resultFile);
        Js.log("success : " ++ filePath)
      }
  )
};

let rec convertRecursion = (filePath, removePathArray) =>
  filePath
  |> Node.Fs.existsSync
  |> (
    (result) =>
      switch result {
      | false => ExcepetionHandleSystem.throwMessage({j|the $filePath should exist|j})
      | true =>
        filePath
        |> Node.Fs.readdirSync
        |> WonderCommonlib.ArraySystem.forEach(
           [@bs]  (fileName: string) => {
               let fileDir = fileName |> PathExtend.join(filePath);
               fileDir
               |> statSync
               |> (
                 (stats) =>
                   if (statsToJsObj(stats)##isFile()) {
                     fileDir
                     |> PathExtend.parse
                     |> (
                       (pathObj) => {
                         let ext = PathExtend.parseTojsObj(pathObj)##ext;
                         switch ext {
                         | ".js" => fileDir |> _parseSystem(removePathArray)
                         | _ => ()
                         }
                       }
                     )
                   } else if (statsToJsObj(stats)##isDirectory()) {
                     convertRecursion(fileName |> PathExtend.join(filePath), removePathArray)
                   }
               )
             }
           )
      }
  );

let convert = (filePath, ~removePathArray=?) =>
  switch removePathArray {
  | None => [|"lib", "es6_global"|] |> convertRecursion(filePath)
  | Some(value) => value |> convertRecursion(filePath)
  };