;;; tceasm-mode-el -- Major mode for editing TCE assembler files

;; Author: Mikael Lepistö <elhigu@gmail.com>
;; Created: 14 Mar 2006
;; Keywords: TCE assembler mode
;;
;;; Commentary:
;;
;; This mode is modified from Scott Andrew Bortons example
;; WPDL file mode. http://two-wugs.net/emacs/mode-tutorial.html

;;; Code:
(defvar tceasm-mode-hook nil)
(defvar tceasm-mode-map
  (let ((tceasm-mode-map (make-keymap)))
    (define-key tceasm-mode-map "\C-j" 'newline-and-indent)
    tceasm-mode-map)
  "Keymap for TCE assembler mode")

(add-to-list 'auto-mode-alist '("\\.tceasm\\'" . tceasm-mode))

;; different fonts for syntax hiliting.
;;font-lock-builtin-face
;;font-lock-comment-face
;;font-lock-constant-face
;;font-lock-doc-face
;;font-lock-function-name-face
;;font-lock-keyword-face
;;font-lock-string-face
;;font-lock-type-face
;;font-lock-variable-name-face
;;font-lock-warning-face

(defconst tceasm-font-lock-keywords-1
  (list
   ;; DA, DATA, CODE and directives are recognized as reserved words..
   ;; < and > means that key words needs white space or beginning of line for being hilited..
   '("\\<\\(DATA\\|DA\\|CODE\\)\\>\\|\\(^[\t\\ ]*:[a-zA-Z0-9_]*\\>\\)" . font-lock-builtin-face)
   
   ;; label coloring
   '("\\<[a-zA-Z_][a-zA-Z0-9_]*:" . font-lock-keyword-face)
   )
  "Minimal highlighting expressions for tceasm mode.")

;; I should define strings containing regexps for constants etc...

(defconst tceasm-font-lock-keywords-2
  (append tceasm-font-lock-keywords-1
          (list
           ;; recognition order does matter. First regexp is recognized first and then the second etc..
           ;; if one part is recognized once, it can't match again for later regexps..
           
           ;; guard recognition
           '("[?!][\t\\ ]*\\(\\([a-zA-Z_][a-zA-Z0-9_]*\\.\\([a-zA-Z_][a-zA-Z0-9_]*\\.\\)*[0-9]+\\)\\|\\([a-zA-Z_][a-zA-Z0-9_]*\\(\\.[a-zA-Z_][a-zA-Z0-9_]*\\)+\\)\\)" . font-lock-type-face)
           
           ;; these two must be recognized before last ones for making expression and constant recognition to work..
           ;; I should find some variable or something that marks these patterns to be default font...
           ;; unit.[port.]index references
           '("[a-zA-Z_][a-zA-Z0-9_]*\\.\\([a-zA-Z_][a-zA-Z0-9_]*\\.\\)*[0-9]+" . font-lock-doc-face)
           
           ;; unit.port or unit.port.opcode references
           '("[a-zA-Z_][a-zA-Z0-9_]*\\(\\.[a-zA-Z_][a-zA-Z0-9_]*\\)+" . font-lock-doc-face)
           
           ;; expression recognition
           '("\\(,\\|\\b\\)[a-zA-Z_][a-zA-Z0-9_]*\\(\\[\t\\ ]*+[\t\\ ]*\\(\\(0x\\|0X\\|0b\\|0B\\)[a-fA-F0-9]+\\)\\|\\(\\(+\\|-\\)?[0-9]+\\)\\)?\\([\t\\ ]*=[\t\\ ]*\\)?" . font-lock-function-name-face)
           
           ;; constant recognition
           '("\\(\\(0x\\|0X\\|0b\\|0B\\)[a-fA-F0-9]+\\)\\|\\(\\(+\\|-\\)?[0-9]+\\)" . font-lock-constant-face)
           
           ;; long immediate recognition
           ;;           '("\\[.*\\]" . font-lock-constant-face)
           
           ))
  
  "Additional Keywords to highlight in tceasm mode.")

(defconst tceasm-font-lock-keywords-3
  (append tceasm-font-lock-keywords-2
          (list
           ;; add some more syntax recognition here if you want to
           
           ))
  "Balls-out highlighting in tceasm mode.")

(defvar tceasm-font-lock-keywords tceasm-font-lock-keywords-3
  "Default highlighting expressions for tceasm mode.")


;; Indentation rules:

;; 1. labels, directives and CODE and DATA declarations are not indented

;; 2. after directives and section start declarations row is indented by one tab-width

;; 3. after labels same that rule 2, but if there is instruction or DA 
;;    definition after label, then next line is indented to same position
;;    with line after label
;;
;; e.g. 
;; label: instruction ;
;;        next-instruction;

;; 4. DA definitions whose init data fields are spread over multiple lines
;;    are indented as shown in following example:
;;
;;    DA 4 2:6
;;         2:5 ;
;;
;; 5. Comments are indeted to start from the same position where next line starts

(defun tceasm-indent-line ()
  "Indent current line as TCE assembler code."
  (interactive)
  (beginning-of-line)
  (if (bobp)
	  (indent-line-to 0)           ; First line is always non-indented
	(let ((not-indented t) cur-indent)
      ;; If directive, label or section declaration set indentation to zero..
	  (if (looking-at "^[ \t]*\\(DATA\\|CODE\\|\\([a-zA-Z_][a-zA-Z0-9_]*:\\)\\|:\\)")
          (indent-line-to 0)
        
        ;; else if comment, check indentation of next line and indent to same level
        (if (looking-at "^[ \t]*#")
            (progn
              (save-excursion              
                (setq not-indented 1)
                (while not-indented
                  (forward-line 1)
                  (if (looking-at "^[ \t]*$")
                      (setq not-indented 1)
                    (setq not-indented nil)                    
                    ))
                (setq cur-indent (current-indentation))
                )
              (indent-line-to cur-indent))
          
          ;; else let's find previous non-empty line and decide what to do
          (progn
            (setq not-indented 1)
            (save-excursion              
              ;; find previous non-empty line
              (while not-indented
                (forward-line -1)
                (if (looking-at "^[ \t]*$")
                    (setq not-indented 1)
                  (setq not-indented nil)                  
                  ))
              
              ;; if DA size line that has init data and its init data is spread to multiple lines..
              (if (and (looking-at "^.*[ \t:]+DA[ \t]+[0-9]+[ \t]+[-0-9][^;]*$")
                       (looking-at "^.*[ \t:]+DA[ \t]+[0-9]+[ \t]+[-0-9]")) ; this just checks right position where to indent
                  (setq cur-indent (- (match-end 0) (match-beginning 0) 1))
                
                ;; if previous line was DATA or CODE keyword
                (if (looking-at "^[ \t]*\\(DATA\\|CODE\\|:\\)")
                    (setq cur-indent default-tab-width)
                  
                  ;; if label and instruction or data-line after label find indentation position...
                  (if (looking-at "^[ \t]*[a-zA-Z_][a-zA-Z0-9_]*:[ \t]*[a-zA-Z0-9_]")
                      (setq cur-indent (- (match-end 0)
                                          (match-beginning 0) 1))
                    
                    ;; if just label one tab indentation
                    (if (looking-at "^[ \t]*[a-zA-Z_][a-zA-Z0-9_]*:[ \t]*")
                        (setq cur-indent default-tab-width)
                      ;; else keep same indentation that last line
                      (setq cur-indent (current-indentation)))))))
            (indent-line-to cur-indent))
          ))
      )))

(defvar tceasm-mode-syntax-table
  (let ((tceasm-mode-syntax-table (make-syntax-table)))
    
    ;; underscore is valid for entity names
    (modify-syntax-entry ?_ "w" tceasm-mode-syntax-table)
    
    ;; Punctuation marks
    (modify-syntax-entry ?\; "." tceasm-mode-syntax-table)
    (modify-syntax-entry ?, "." tceasm-mode-syntax-table)
    (modify-syntax-entry ?. "." tceasm-mode-syntax-table)
    
    ;; Long immediate parenthsis
    (modify-syntax-entry ?\[ "(" tceasm-mode-syntax-table)
    (modify-syntax-entry ?\] ")" tceasm-mode-syntax-table)
    
    ;; Some white spaces
    (modify-syntax-entry ?\ " " tceasm-mode-syntax-table)
    (modify-syntax-entry ?\t " " tceasm-mode-syntax-table)
    (modify-syntax-entry ?\n " " tceasm-mode-syntax-table)
    
    ;; One line comments
    (modify-syntax-entry ?# "<" tceasm-mode-syntax-table)
    (modify-syntax-entry ?\n ">" tceasm-mode-syntax-table)
    tceasm-mode-syntax-table)
  "Syntax table for tceasm-mode")

(defun tceasm-mode ()
  (interactive)
  (kill-all-local-variables)
  (use-local-map tceasm-mode-map)
  (set-syntax-table tceasm-mode-syntax-table)
  ;; Set up font-lock
  (set (make-local-variable 'font-lock-defaults) '(tceasm-font-lock-keywords))
  ;; Register our indentation function
  (set (make-local-variable 'indent-line-function) 'tceasm-indent-line)
  (setq major-mode 'tceasm-mode)
  (setq mode-name "tceasm")
  (run-hooks 'tceasm-mode-hook)
)

(provide 'tceasm-mode)

;;; tceasm-mode.el ends here
