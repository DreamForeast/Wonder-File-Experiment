open Wonder_jest;

let _ =
  describe(
    "convert import css",
    () => {
      open Expect;
      /* open Expect.Operators; */
      open Sinon;
      let sandbox = getSandboxDefaultVal();
      let getConvertedPath = (index) => {
        let filePath = "./lib/js/test/res/testImportFile.js";
        ConvertImportCss.convert("./lib/js/test", ~removePathArray=[|"lib", "js"|]);
        Node.Fs.readFileSync(filePath, `utf8)
        |> Js.String.split("\n")
        |> Js.Array.map(
             (line) =>
               switch ([%re {|/^import\s*([\"|\'](.*)[\"|\'])/g|}] |> Js.Re.exec(line)) {
               | None => ""
               | Some(result) =>
                 Js.Re.matches(result)[index];
               }
           )
        |> Js.Array.filter((item) => item !== "")
        |> ArraySystem.getFirst
      };
      beforeEach(() => sandbox := createSandbox());
      afterEach(() => restoreSandbox(refJsObjToSandbox(sandbox^)));
      test(
        "replace the css path with relative path",
        () => {
          let resultPath = "../../../../test/res/css/app.css";
          getConvertedPath(2) |> expect |> toEqual(resultPath)
        }
      );
      test(
        {|repalce "importCss('xxx')" with "import 'xxx'"|},
        () => {
          let result = "import '../../../../test/res/css/app.css'";
          getConvertedPath(0) |> expect |> toEqual(result)
        }
      )
    }
  );