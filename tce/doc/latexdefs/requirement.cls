\ProvidesClass{requirement}

% The style is taken from the global style file of the tce.
\input{tcedocs.cls}

% Define the type of the document.
\doctype{Functional Requirements}
% Code for the document type
% S = specifications (functional requirements, architecture, design)
\newcommand{\thedoccode}[0]{S}


% Macros for functional requirements.

% Text before usage to represent the style used in the 
\newcommand{\usagestyle}[1]{In addition to the standard command line
options accepted by every application of the TCE toolset, the #1
accepts the command line options listed below. The first column
indicates the long option which is used by prepending it by two
hyphens: `--'. The second column contains the short option used by one
prepending hyphen: `-'. For each option, the third column indicates
the type of argument (if any) of the option: \emph{i} (integer
number), \emph{k} (keyword), \emph{s} (string), \emph{f} (fractional
number), \emph{b} (no option, that is, boolean). A Boolean option
disables the flag it controls if the long option is prepended by
`no-'. Unless otherwie stated, other option types cannot be prepended
by the `no-' prefix.}

%%% -------------------------------------------------
%%% Environment for Context-Free Grammars
%%% -------------------------------------------------

%% definitions (can be used inside any list environment)

%% grammar rule (also called production)
\newcommand{\grrule}[1]{\item[\emph{#1}~:]~\\}
%% grammar symbol (either terminal or nonterminal)
\newcommand{\grsym}[1]{\mbox{\emph{#1}}}
%% literal: grammar terminal symbol
\newcommand{\grlit}[1]{`\mbox{#1}'}
%% alternative expansion pattern
\newcommand{\gror}{|~}
%% empty expansion pattern (called epsilon)
\newcommand{\grepsilon}{\emph{EPSILON}}
%% optional grammar symbol
\newcommand{\gropt}[1]{\mbox{#1}\ensuremath{\!\!_{~\mathrm{opt}\,}}}

%%% Context Free Grammar Environment
\newenvironment{grammar}{%
\begin{list}{}{}}%
{\end{list}}
