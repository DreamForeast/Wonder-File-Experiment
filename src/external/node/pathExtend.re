[@bs.module "path"] external extname : string => string = "";

[@bs.module "path"] external join : (string, string) => string = "";

[@bs.module "path"] external relative : (string, string) => string = "";

type parseType;

[@bs.module "path"] external parse : string => parseType = "";

external parseTojsObj : parseType => Js.t({..}) = "%identity";