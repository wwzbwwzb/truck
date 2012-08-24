"2012-08-24
set viminfo='1000,f1,<500

"2010-10-09
"------------------------------------------------
set tags=tags
"display lone line friendly
set display=lastline
"set autochdir

set mouse=a
syntax on
set nu
map <F6> :set nu!<CR>
map <F7> :set list!<CR>
map <F7> :set wrap!<CR>
"set list
" for compile android 4.0
set makeprg={\ cd\ $c;\ .\ build/envsetup.sh;\ cd\ -;\ TOP=$c;\ mm\ ;beep;\ }
map <F1> :f
map <F9> :make
map <F10> :cw

" shift tab pages, can't use in putty
map <S-left> :tabp
map <S-right> :tabn
map < :tabp<CR>
map > :tabn<CR>

"----------------------------------------
" Return to last edit position (You want this!) *N*
autocmd BufReadPost *
     \ if line("'\"") > 0 && line("'\"") <= line("$") |
     \   exe "normal! g`\"" |
     \ endif

"set expandtab
" can't edit makefile
set sw=2
set ts=2

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

" define the color of Comment
:hi Comment  term=bold ctermfg=6 guifg=Blue

set mouse=a
set incsearch

" use make to find errors and warnnings 

set nu!
set nu
":map <F3> :vs /home/gwj/docs.txt <CR>

set hlsearch
set cindent
set autoindent

map <F4> zf%
map <F5> zo
map T :r! date \+\%F<CR>o<Tab>


set nu!
set nu
":map <F3> :vs /home/gwj/docs.txt <CR>

set hlsearch
set cindent
set autoindent

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
	endif



	map <C-_> :cstag <C-R>=expand("<cword>")<CR><CR>

"A couple of very commonly used cscope queries (using ":cs find") is to
"find all functions calling a certain function and to find all occurrences
"of a particular C symbol.  To do this, you can use these mappings as an
"example: >

"	map g<C-]> :cs find 3 <C-R>=expand("<cword>")<CR><CR>
"	map g<C-\> :cs find 0 <C-R>=expand("<cword>")<CR><CR>

"These mappings for Ctrl-] (right bracket) and Ctrl-\ (backslash) allow you to
"place your cursor over the function name or C symbol and quickly query cscope
"for any matches.

"Or you may use the following scheme, inspired by Vim/Cscope tutorial from
"Cscope Home Page (http://cscope.sourceforge.net/): >

	nmap <C-_>s :cs find s <C-R>=expand("<cword>")<CR><CR>
	nmap <C-_>g :cs find g <C-R>=expand("<cword>")<CR><CR>
	nmap <C-_>c :cs find c <C-R>=expand("<cword>")<CR><CR>
	nmap <C-_>t :cs find t <C-R>=expand("<cword>")<CR><CR>
	nmap <C-_>e :cs find e <C-R>=expand("<cword>")<CR><CR>
	nmap <C-_>f :cs find f <C-R>=expand("<cfile>")<CR><CR>
	nmap <C-_>i :cs find i ^<C-R>=expand("<cfile>")<CR>$<CR>
	nmap <C-_>d :cs find d <C-R>=expand("<cword>")<CR><CR>

	" Using 'CTRL-spacebar' then a search type makes the vim window
	" split horizontally, with search result displayed in
	" the new window.

	nmap <C-Space>s :scs find s <C-R>=expand("<cword>")<CR><CR>
	nmap <C-Space>g :scs find g <C-R>=expand("<cword>")<CR><CR>
	nmap <C-Space>c :scs find c <C-R>=expand("<cword>")<CR><CR>
	nmap <C-Space>t :scs find t <C-R>=expand("<cword>")<CR><CR>
	nmap <C-Space>e :scs find e <C-R>=expand("<cword>")<CR><CR>
	nmap <C-Space>f :scs find f <C-R>=expand("<cfile>")<CR><CR>
	nmap <C-Space>i :scs find i ^<C-R>=expand("<cfile>")<CR>$<CR>
	nmap <C-Space>d :scs find d <C-R>=expand("<cword>")<CR><CR>

	" Hitting CTRL-space *twice* before the search type does a vertical
	" split instead of a horizontal one

	nmap <C-Space><C-Space>s
		\:vert scs find s <C-R>=expand("<cword>")<CR><CR>
	nmap <C-Space><C-Space>g
		\:vert scs find g <C-R>=expand("<cword>")<CR><CR>
	nmap <C-Space><C-Space>c
		\:vert scs find c <C-R>=expand("<cword>")<CR><CR>
	nmap <C-Space><C-Space>t
		\:vert scs find t <C-R>=expand("<cword>")<CR><CR>
	nmap <C-Space><C-Space>e
		\:vert scs find e <C-R>=expand("<cword>")<CR><CR>
	nmap <C-Space><C-Space>i
		\:vert scs find i ^<C-R>=expand("<cfile>")<CR>$<CR>
	nmap <C-Space><C-Space>d
		\:vert scs find d <C-R>=expand("<cword>")<CR><CR>


map } :cs find s <cword> <CR>

