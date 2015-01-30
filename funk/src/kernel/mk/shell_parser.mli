type token =
  | EOL
  | STRING of (string)

val args :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> string list
