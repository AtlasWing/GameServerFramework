set noexpandtab
set shiftwidth=4
set tabstop=4
set softtabstop=4
set nobackup
set cindent
set autoindent
set showcmd
set helplang=Cn
set nofoldenable
set noswapfile
set number
set mouse=nv
set hlsearch
set incsearch
set viminfo+=h
set fileencodings=utf-8,gb2312,gbk,gb18030

set nocp
filetype plugin on

autocmd FileType perl set keywordprg=perldoc\ -f

nmap <F5> ^W_^W\|
nmap <F6> ^W=
imap <F5> <ESC>^W_^W\|a
imap <F6> <ESC>^W=a
nmap gF ^Wf

syntax on

colorscheme evening
hi Normal ctermfg=grey ctermbg=black
hi Visual ctermfg=green ctermbg=black
hi Search term=reverse cterm=standout ctermfg=green  ctermbg=yellow
hi IncSearch term=reverse cterm=standout ctermfg=green ctermbg=yellow
hi PmenuSel ctermbg=Green ctermfg=Yellow
