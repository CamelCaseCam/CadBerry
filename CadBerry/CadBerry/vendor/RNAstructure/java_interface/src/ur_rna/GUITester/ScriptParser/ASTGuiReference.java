/* Generated By:JJTree: Do not edit this line. ASTGuiReference.java Version 6.0 */
/* JavaCCOptions:MULTI=true,NODE_USES_PARSER=false,VISITOR=false,TRACK_TOKENS=false,NODE_PREFIX=AST,NODE_EXTENDS=,NODE_FACTORY=,SUPPORT_CLASS_VISIBILITY_PUBLIC=true */
 package ur_rna.GUITester.ScriptParser;

public class ASTGuiReference extends ScriptNode {
  public ScriptNode guiItem; /* guiItem should be either a ASTGuiSearchCriteria or a ASTVarRef */
  public int relationshipKind;
  public ASTGuiReference relative;

  public ASTGuiReference(int id) {
    super(id);
  }

  public ASTGuiReference(GuiTestScriptParser p, int id) {
    super(p, id);
  }

}
/* JavaCC - OriginalChecksum=f29ca7cc1f3c94b190acebfac3943e6b (do not edit this line) */
