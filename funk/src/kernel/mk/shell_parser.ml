type token =
  | EOL
  | STRING of (string)

open Parsing;;
let yytransl_const = [|
  257 (* EOL *);
    0|]

let yytransl_block = [|
  258 (* STRING *);
    0|]

let yylhs = "\255\255\
\001\000\001\000\000\000"

let yylen = "\002\000\
\002\000\001\000\002\000"

let yydefred = "\000\000\
\000\000\000\000\002\000\000\000\003\000\001\000"

let yydgoto = "\002\000\
\005\000"

let yysindex = "\001\000\
\255\254\000\000\000\000\255\254\000\000\000\000"

let yyrindex = "\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000"

let yygindex = "\000\000\
\255\255"

let yytablesize = 3
let yytable = "\003\000\
\004\000\001\000\006\000"

let yycheck = "\001\001\
\002\001\001\000\004\000"

let yynames_const = "\
  EOL\000\
  "

let yynames_block = "\
  STRING\000\
  "

let yyact = [|
  (fun _ -> failwith "parser")
; (fun parser_env ->
    let _1 = (peek_val parser_env 1 : string) in
    let _2 = (peek_val parser_env 0 : string list) in
    Obj.repr(
# 35 "mk/shell_parser.mly"
                  ( _1 :: _2 )
# 59 "mk/shell_parser.ml"
               : string list))
; (fun parser_env ->
    Obj.repr(
# 36 "mk/shell_parser.mly"
          ( [] )
# 65 "mk/shell_parser.ml"
               : string list))
(* Entry args *)
; (fun parser_env -> raise (YYexit (peek_val parser_env 0)))
|]
let yytables =
  { actions=yyact;
    transl_const=yytransl_const;
    transl_block=yytransl_block;
    lhs=yylhs;
    len=yylen;
    defred=yydefred;
    dgoto=yydgoto;
    sindex=yysindex;
    rindex=yyrindex;
    gindex=yygindex;
    tablesize=yytablesize;
    table=yytable;
    check=yycheck;
    error_function=parse_error;
    names_const=yynames_const;
    names_block=yynames_block }
let args (lexfun : Lexing.lexbuf -> token) (lexbuf : Lexing.lexbuf) =
   (yyparse yytables 1 lexfun lexbuf : string list)
