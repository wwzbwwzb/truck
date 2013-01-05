"2012-08-24
set viminfo='1000,f1,<500


"显示函数名称，c/java
" set verbose=9 可以调试命令
fun! ShowFuncName()
	let lnum = line(".")
	let col = col(".")
	echohl ModeMsg
" for c
  if &filetype == "c" || &filetype == "cpp"
	  echo getline(search("^[^ \t#/]\\{2}.*[^:]\s*$", 'bW')) 
" for java
	elseif  &filetype == "java"
  	echo getline(search("\\h\\+\\s\\+\\h\\+\\s*(.*)", 'bW')) 
  else
	  echo getline(search("^[^ \t#/]\\{2}.*[^:]\s*$", 'bW')) 
	endif
	echohl None
"	call search("\\%" . lnum . "l" . "\\%" . col . "c")
	call cursor(lnum,col)
endfun

map <F12> :call  ShowFuncName() <CR>
au  CursorMoved *    call  ShowFuncName()



"2010-10-09
"------------------------------------------------
"对长行使用gk  gj 跳转
set tags=tags
"display lone line friendly
set display=lastline
"set autochdir

" define the color of Comment
"hi Comment  term=bold ctermfg=6 guifg=Blue
hi Identifier ctermfg=LightBlue  
hi Comment  ctermfg=yellow

" from chia-I
"colorscheme delek
"set pt=<F9>
set copyindent
let c_space_errors=1

au  FileType python  set sw=4 sts=4 et
au  FileType lua     set sw=4 sts=4 et

set mouse=a
syntax on
set nu
" can't edit makefile
set sw=2
set ts=2
"set expandtab set et
set encoding=utf-8
set incsearch
set hlsearch
set cindent
set autoindent

map <F4> zf%
map <F5> zo
map <F6> :set nu!<CR>
map <F7> :set list!<CR>
map <F8> :set wrap!<CR>
"set list
" for compile android 4.0
set makeprg={\ cd\ $c;\ .\ build/envsetup.sh;\ cd\ -;\ TOP=$c;\ mm\ ;beep;\ }
map <F9> :make<CR>
map <F10> :make<CR>

" shift tab pages, can't use in putty
map < :tabp<CR>
map > :tabn<CR>
map T :r! date \+\%F<CR>o<Tab>

"----------------------------------------
" Return to last edit position (You want this!) *N*
autocmd BufReadPost *
     \ if line("'\"") > 0 && line("'\"") <= line("$") |
     \   exe "normal! g`\"" |
     \ endif

map <F3> :Tlist<CR>
map ts :ts<CR>
map tn :tn
map tp :tp
map J :cn<CR>
map , :cn<CR>
map . :cp<CR>
map K :cp<CR>
map O :cl!<CR>
map L :cc!<CR>
map q :q<CR>

if has("cscope")
"		set csprg=/usr/local/bin/cscope
  map <C-e> :cs find c <cword><CR>
	set csto=0
	set cst
	set nocsverb
	" add any database in current directory
	if filereadable("cscope.out")
	    cs add cscope.out
	" else add database pointed to by environment
	elseif $CSCOPE_DB != ""
	    cs add $CSCOPE_DB
	endif
	set csverb

"These mappings for Ctrl-] (right bracket) and Ctrl-\ (backslash) allow you to
	nmap <C-[>s :cs find s <C-R>=expand("<cword>")<CR><CR>
  "find a string in project
  map } :cs find s <cword> <CR>
	nmap <C-[>g :cs find g <C-R>=expand("<cword>")<CR><CR>
	nmap <C-[>c :cs find c <C-R>=expand("<cword>")<CR><CR>
	nmap <C-[>t :cs find t <C-R>=expand("<cword>")<CR><CR>
	nmap <C-[>e :cs find e <C-R>=expand("<cword>")<CR><CR>
	nmap <C-[>f :cs find f <C-R>=expand("<cfile>")<CR><CR>
	nmap <C-[>i :cs find i ^<C-R>=expand("<cfile>")<CR>$<CR>
	nmap <C-[>d :cs find d <C-R>=expand("<cword>")<CR><CR>
endif "cscope 

