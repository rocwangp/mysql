<!DOCTYPE html>
<html lang="zh-CN">


<head>
	<link rel="stylesheet" href="https://cdn.bootcss.com/bootstrap/3.3.7/css/bootstrap.min.css">
	<script src="https://cdn.bootcss.com/jquery/2.1.1/jquery.min.js"></script>
    <script src="https://cdn.bootcss.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">  
	
    <title>Social Network</title>
    
    
    <script type="text/javascript">
    	function hideBooksAndAuthorsDiv(){
    		$("#books_div").hide();
    		$("#authors_div").hide();
    		$("#messages_div").hide();
            $("#friend_requests_div").hide();
            $("#friends_div").hide();
            $("#groups_div").hide();
    	}
    </script>
</head>
<body onLoad="hideBooksAndAuthorsDiv()">

<nav class="navbar navbar-default" role="navigation">
    <div class="container-fluid">
        <div class="navbar-header">
            <button type="button" class="navbar-toggle" data-toggle="collapse"
                    data-target="#example-navbar-collapse">
                <span class="sr-only">切换导航</span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
            </button>
            <a class="navbar-brand">社交网络</a>
        </div>
        <div class="collapse navbar-collapse" id="example-navbar-collapse">
            <ul class="nav navbar-nav navbar-left">
            
                
            	<li class="dropdown">
                    <a href="#" class="dropdown-toggle" data-toggle="dropdown">
                        用户管理
                        <b class="caret"></b>
                    </a>
                    <ul class="dropdown-menu">
                  	  <li class="divider"></li>
                        <li><a onclick="onListUserButtonClicked()">全部用户</a></li>
                        <li class="divider"></li>
                        <li><a  onclick="onAddUserButtonClicked()" data-toggle="modal" data-target="#addbook">注册用户</a></li>
                        <li class="divider"></li>
                        <li><a  onclick="onQueryUserButtonClicked()" data-toggle="modal" data-target="#querybook">查询用户</a></li>
                        <li class="divider"></li>
                        <li><a  onclick="onUserLoginButtonClicked()" data-toggle="modal" data-target="#userlogin">登录</a></li>
                        <li class="divider"></li>
                        <li><a  onclick="onListFriendRequestButtonClicked()" data-toggle="modal">好友请求</a></li>
                        <li class="divider"></li>
                        <li><a  onclick="onAddGroupButtonClicked()" data-toggle="modal" data-target="#addgroup">添加分组</a></li>
                        <li class="divider"></li>
                        <li><a  onclick="onListFriendButtonClicked()" data-toggle="modal" data-target="#queryfriends">查询好友</a></li>
                        <li class="divider"></li>
                        <li><a  onclick="onListGroupButtonClicked()" data-toggle="modal">查询分组</a></li>
                        <li class="divider"></li>
                        <li><a  onclick="onQueryChatMessageButtonClicked()" data-toggle="modal">查询聊天消息</a></li>
                        <li class="divider"></li>
                        <li><a  onclick="onModifyUserButtonClicked()" data-toggle="modal" data-target="#modifyuser">修改个人信息</a></li>
                    </ul>
                </li>
				
				<li class="dropdown">
					<a href="#" class="dropdown-toggle" data-toggle="dropdown">
					作者管理
						<b class="caret"></b>
					</a>
					<ul class="dropdown-menu">
						<li class="divider"></li>
						<li><a  onclick="onListAuthorButtonClicked()">全部作者</a></li>
						<li class="divider"></li>
						<li><a  onclick="onAddAuthorButtonClicked()" data-toggle="modal" data-target="#addauthor">增加作者</a></li>
						<li class="divider"></li>
						<li><a  onclick="onQueryAuthorButtonClicked()" data-toggle="modal" data-target="#queryauthor">查询作者</a></li>
					</ul>
            </ul>
        </div>
    </div>
</nav>

<!-- 用户登录 模态框 -->
<div class="modal fade" id="userlogin" tabindex="-1" role="dialog" aria-labelledby="userlogin" aria-hidden="true"  data-backdrop="static">
	<div class="modal-dialog">
		<div class="modal-content">
			<div class="modal-header">
				<button type="button" class="close" data-dismiss="modal" aria-hidden="true">
					&times;
				</button>
				<h4 class="modal-title" id="query_form_header">
				登录
				</h4>
			</div>
			
			<div class="modal-body">
				<div class="form-group">
					<label for="user_id" class="col-sm-3 control-label">用户名</label>
					<div class="col-sm-9">
						<input type="text" class="form-control" id="user_login_id" name="user_id">
					</div>
					<label for="password" class="col-sm-3 control-label">密码</label>
					<div class="col-sm-9">
						<input type="text" class="form-control" id="user_login_password" name="password">
					</div>
				</div>
				<div class="modal-footer">
					<button type="button" class="btn btn-default" data-dismiss="modal">关闭
					</button>
					<button onclick="onUserLoginSubmitButtonClicked()" type="submit" class="btn btn-primary">提交
					</button><span id="user_login_submit_btn"></span>
				</div>
			</div>
		</div>
	</div>
</div>


<!-- 已登录用户（Modal） -->
<div class="modal fade" id="logining_user" tabindex="-1" role="dialog" aria-labelledby="logining_user" aria-hidden="true"  data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="book_modify_header">
                    已登录用户
                </h4>
            </div>
            <div class="modal-body">
              
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">user_id</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="logining_user_id" name="user_id"
                                   >
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label for="isbn" class="col-sm-3 control-label">password</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="logining_user_passowrd" name="password"
                                   >
                        </div>
                    </div>
                   
                    <div class="form-group">
                        <label for="authorId" class="col-sm-3 control-label">nickname</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" name="nickname"  id="logining_user_nickname"
                                   >
                        </div>
                    </div>
                    <div class="form-group">
                        <label for="publisher" class="col-sm-3 control-label">fullname</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" name="fullname"  id="logining_user_fullname"
                                   >
                        </div>
                    </div>

                    <div class="form-group">
                        <label for="publishDate" class="col-sm-3 control-label">sex</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" name="sex" id="logining_user_sex"
                                   >
                        </div>
                    </div>
          
                                  <div class="form-group">
                        <label for="price" class="col-sm-3 control-label">email</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" name="email"  id="logining_user_email"
                                   >
                        </div>
                    </div>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>

<!-- 用户信息（Modal） -->
<div class="modal fade" id="modifyuser" tabindex="-1" role="dialog" aria-labelledby="friendrequest" aria-hidden="true"  data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="book_modify_header">
                    修改个人信息
                </h4>
            </div>
            <div class="modal-body">
              
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">用户名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="modify_user_user_id" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">密码</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="modify_user_password" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">昵称</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="modify_user_nickname" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">姓名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="modify_user_fullname" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">性别</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="modify_user_sex" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">邮箱</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="modify_user_email" name="title"
                                   >
                        </div>
                    </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                <button onclick="onModifyUserSubmitButtonClicked()" type="submit" class="btn btn-primary">提交
                </button>
                <span id="friend_request_agree_btn"></span>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>

<!-- 好友请求（Modal） -->
<div class="modal fade" id="friendrequest" tabindex="-1" role="dialog" aria-labelledby="friendrequest" aria-hidden="true"  data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="book_modify_header">
                    好友请求
                </h4>
            </div>
            <div class="modal-body">
              
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">请求id</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="friend_request_id" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">申请用户</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="friend_request_sender" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">目标用户</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="friend_request_recver" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">申请用户分组</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="friend_request_sender_group" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">分组</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="friend_request_group_name" name="title"
                                   >
                        </div>
                    </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                <button onclick="onAgreeFriendRequestSubmitButtonClicked()" type="submit" class="btn btn-primary">提交
                </button>
                <span id="friend_request_agree_btn"></span>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>


<!-- 选择分组（Modal） -->
<div class="modal fade" id="choosegroup" tabindex="-1" role="dialog" aria-labelledby="choosegroup" aria-hidden="true"  data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="book_modify_header">
                    选择分组
                </h4>
            </div>
            <div class="modal-body">
              
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">用户名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="choose_group_user_id" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">好友名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="choose_group_friend_id" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">分组名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="choose_group_group_name" name="title"
                                   >
                        </div>
                    </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                <button onclick="onSendFriendRequestSubmitButtonClicked()" type="submit" class="btn btn-primary">提交
                </button>
                <span id="friend_request_agree_btn"></span>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>
<!-- 用户查询 模态框 -->
<div class="modal fade" id="querybook" tabindex="-1" role="dialog" aria-labelledby="querybook" aria-hidden="true"  data-backdrop="static">
	<div class="modal-dialog">
		<div class="modal-content">
			<div class="modal-header">
				<button type="button" class="close" data-dismiss="modal" aria-hidden="true">
					&times;
				</button>
				<h4 class="modal-title" id="query_form_header">
				查询用户
				</h4>
			</div>
			
			<div class="modal-body">
				<div class="form-group">
					<label for="book_name" class="col-sm-3 control-label">用户名</label>
					<div class="col-sm-9">
						<input type="text" class="form-control" id="book_query_title" name="title">
					</div>
				</div>
				<div class="modal-footer">
					<button type="button" class="btn btn-default" data-dismiss="modal">关闭
					</button>
					<button onclick="onQueryUserSubmitButtonClicked()" type="submit" class="btn btn-primary">提交
					</button><span id="book_query_submit_btn"></span>
				</div>
			</div>
		</div>
	</div>
</div>

<!-- 修改好友所在分组 模态框 -->
<div class="modal fade" id="modifyfriendgroup" tabindex="-1" role="dialog" aria-labelledby="querybook" aria-hidden="true"  data-backdrop="static">
	<div class="modal-dialog">
		<div class="modal-content">
			<div class="modal-header">
				<button type="button" class="close" data-dismiss="modal" aria-hidden="true">
					&times;
				</button>
				<h4 class="modal-title" id="query_form_header">
                    修改好友所在分组
				</h4>
			</div>
			
			<div class="modal-body">
				<div class="form-group">
					<label for="book_name" class="col-sm-3 control-label">好友名</label>
					<div class="col-sm-9">
						<input type="text" class="form-control" id="modify_friend_group_friend_id" name="title">
					</div>
				</div>
				<div class="form-group">
					<label for="book_name" class="col-sm-3 control-label">分组名</label>
					<div class="col-sm-9">
						<input type="text" class="form-control" id="modify_friend_group_group_name" name="title">
					</div>
				</div>
				<div class="modal-footer">
					<button type="button" class="btn btn-default" data-dismiss="modal">关闭
					</button>
					<button onclick="onModifyFriendGroupSubmitButtonClicked()" type="submit" class="btn btn-primary">提交
					</button><span id="book_query_submit_btn"></span>
				</div>
			</div>
		</div>
	</div>
</div>


<!-- 修改分组 模态框 -->
<div class="modal fade" id="modify_group" tabindex="-1" role="dialog" aria-labelledby="querybook" aria-hidden="true"  data-backdrop="static">
	<div class="modal-dialog">
		<div class="modal-content">
			<div class="modal-header">
				<button type="button" class="close" data-dismiss="modal" aria-hidden="true">
					&times;
				</button>
				<h4 class="modal-title" id="query_form_header">
                    修改分组
				</h4>
			</div>
			
			<div class="modal-body">
				<div class="form-group">
					<label for="book_name" class="col-sm-3 control-label">分组id</label>
					<div class="col-sm-9">
						<input type="text" class="form-control" id="modify_group_group_id" name="title">
					</div>
				</div>
				<div class="form-group">
					<label for="book_name" class="col-sm-3 control-label">分组名</label>
					<div class="col-sm-9">
						<input type="text" class="form-control" id="modify_group_group_name" name="title">
					</div>
				</div>
				<div class="modal-footer">
					<button type="button" class="btn btn-default" data-dismiss="modal">关闭
					</button>
					<button onclick="onModifyGroupNameSubmitButtonClicked()" type="submit" class="btn btn-primary">提交
					</button><span id="book_query_submit_btn"></span>
				</div>
			</div>
		</div>
	</div>
</div>
<!-- 添加分组（Modal） -->
<div class="modal fade" id="addgroup" tabindex="-1" role="dialog" aria-labelledby="addgroup" aria-hidden="true" data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
        <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                &times;
            </button>
            <h4 class="modal-title" id="book_add_header">
                           添加分组
            </h4>
        </div>
        
        <div class="modal-body">
            <div class="form-group">
                 <label for="title" class="col-sm-3 control-label">分组名</label>
                  <div class="col-sm-9">
                      <input type="text" class="form-control" id="group_add_name" name="title" value="" placeholder="请输入分组名">
                  </div>
         </div>    
        </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                <button onclick="onAddGroupSubmitButtonClicked()" type="submit" class="btn btn-primary">提交
                </button><span id="group_add_submit_btn"></span>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>

<!-- 删除分组（Modal） -->
<div class="modal fade" id="delete_group" tabindex="-1" role="dialog" aria-labelledby="addgroup" aria-hidden="true" data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
        <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                &times;
            </button>
            <h4 class="modal-title" id="book_add_header">
                   删除分组
            </h4>
        </div>
        
        <div class="modal-body">
            <div class="form-group">
                 <label for="title" class="col-sm-3 control-label">分组id</label>
                  <div class="col-sm-9">
                      <input type="text" class="form-control" id="delete_group_group_id" name="title" value="" placeholder="请输入分组名">
                  </div>
            </div>
            <div class="form-group">
                 <label for="title" class="col-sm-3 control-label">分组名</label>
                  <div class="col-sm-9">
                      <input type="text" class="form-control" id="delete_group_group_name" name="title" value="" placeholder="请输入分组名">
                  </div>
            </div>
            <div class="form-group">
                <label for="title" class="col-sm-3 control-label">好友数</label>
                  <div class="col-sm-9">
                      <input type="text" class="form-control" id="delete_group_friend_counts" name="title" value="" placeholder="请输入分组名">
                  </div>
            </div>
        </div>
        <div class="modal-footer">
            <button type="button" class="btn btn-default" data-dismiss="modal">关闭
            </button>
            <button onclick="onDeleteGroupSubmitButtonClicked()" type="submit" class="btn btn-primary">提交
            </button><span id="group_add_submit_btn"></span>
        </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>
<!-- 添加用户（Modal） -->
<div class="modal fade" id="addbook" tabindex="-1" role="dialog" aria-labelledby="addbook" aria-hidden="true" data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
        <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                &times;
            </button>
            <h4 class="modal-title" id="book_add_header">
                           添加用户
            </h4>
        </div>
        
        <div class="modal-body">
            <div class="form-group">
                 <label for="title" class="col-sm-3 control-label">用户名</label>
                  <div class="col-sm-9">
                      <input type="text" class="form-control" id="book_add_title" name="title" value="" placeholder="请输入书名">
                  </div>
         </div>    
                    
         <div class="form-group">
              <label for="isbn" class="col-sm-3 control-label">密码</label>
               <div class="col-sm-9">
                    <input type="text" class="form-control" id="book_add_isbn" name="isbn" placeholder="请输入ISBN">
                </div>
         </div>
                
         <div class="form-group">
                <label for="authorId" class="col-sm-3 control-label">昵称</label>
                <div class="col-sm-9">
                     <input type="text" class="form-control" name="authorID" value="" id="book_add_authorID" placeholder="请输入作者ID">
                 </div>
          </div>
                 
          <div class="form-group">
               <label for="publisher" class="col-sm-3 control-label">姓名</label>
               <div class="col-sm-9">
                    <input type="text" class="form-control" name="publisher" value="" id="book_add_publisher" placeholder="请输入出版社">
               </div>
          </div>
                  
          <div class="form-group">
               <label for="publishDate" class="col-sm-3 control-label">性别</label>
               <div class="col-sm-9">
  						  <input type="text" class="form-control"  name="publishDate" value="" id="book_add_publishDate" placeholder="请输入日期">
               </div>
          </div>
          
	<div class="form-group">
	     <label for="price" class="col-sm-3 control-label">邮箱</label>
	     <div class="col-sm-9">
	         <input type="text" class="form-control" name="price" value="" id="book_add_price"  placeholder="请输入价格">
	      </div>
	</div>
                   
        </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                <button onclick="onAddUserSubmitButtonClicked()" type="submit" class="btn btn-primary">提交
                </button><span id="book_query_add_btn"></span>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>

<!-- 发送消息（Modal） -->
<div class="modal fade" id="send_message" tabindex="-1" role="dialog" aria-labelledby="modifybook" aria-hidden="true"  data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="book_modify_header">
                    聊天窗口
                </h4>
            </div>
            <div class="modal-body">
              
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">用户名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="send_message_user_id" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">好友名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="send_message_friend_id" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">消息内容</label>
                        <div class="col-sm-9">
                            <textarea style="width:300px;height:100px;" id="send_message_context" name="title"></textarea>
                        </div>
                    </div>
                    
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                <button onclick = "onSendMessageSubmitButtonClicked()" class="btn btn-primary">
                    发送
                </button><span id="book_modify_submit_btn"> </span>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>

<!-- 回复消息（Modal） -->
<div class="modal fade" id="reply_message" tabindex="-1" role="dialog" aria-labelledby="modifybook" aria-hidden="true"  data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="book_modify_header">
                    回复窗口
                </h4>
            </div>
            <div class="modal-body">
              
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">消息id</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="reply_message_message_id" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">好友名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="reply_message_sender" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">用户名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="reply_message_recver" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">消息内容</label>
                        <div class="col-sm-9">
                            <textarea style="width:300px;height:100px;" id="reply_message_context" name="title"></textarea>
                        </div>
                    </div>
                    
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">回复内容</label>
                        <div class="col-sm-9">
                            <textarea style="width:300px;height:100px;" id="reply_message_reply_context" name="title"></textarea>
                        </div>
                    </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                <button onclick = "onReplyMessageSubmitButtonClicked()" class="btn btn-primary">
                    发送
                </button><span id="book_modify_submit_btn"> </span>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>

<!-- 接收消息（Modal） -->
<div class="modal fade" id="detailmessage" tabindex="-1" role="dialog" aria-labelledby="modifybook" aria-hidden="true"  data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="book_modify_header">
                    聊天窗口
                </h4>
            </div>
            <div class="modal-body">
              
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">用户名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="recv_message_user_id" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">好友名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="recv_message_friend_id" name="title"
                                   >
                        </div>
                    </div>
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">消息内容</label>
                        <div class="col-sm-9">
                            <textarea style="width:300px;height:100px;" id="recv_message_context" name="title"></textarea>
                        </div>
                    </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>



   <!-- 删除用户（Modal） -->

<div class="modal fade" id="deletebook" tabindex="-1" role="dialog" aria-labelledby="deletebook" aria-hidden="true" data-backdrop="static">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="book_delete_header">
                    确定要删除吗
                </h4>
            </div>
            <div class="modal-body">
              
                 <div class="form-group">
                        <label for="title" class="col-sm-3 control-label">用户名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="book_delete_title" name="title"
                                   >
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label for="isbn" class="col-sm-3 control-label">密码</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" id="book_delete_isbn" name="isbn"
                                   placeholder="请输入ISBN">
                        </div>
                    </div>
                   
                    <div class="form-group">
                        <label for="authorId" class="col-sm-3 control-label">昵称</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" name="authorID"  id="book_delete_authorID"
                                   placeholder="作者ID">
                        </div>
                    </div>
                    <div class="form-group">
                        <label for="publisher" class="col-sm-3 control-label">姓名</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" name="publisher"  id="book_delete_publisher"
                                   placeholder="出版社">
                        </div>
                    </div>

                    <div class="form-group">
                        <label for="publishDate" class="col-sm-3 control-label">性别</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" name="publishDate" id="book_delete_publishDate"
                                   placeholder="出版日期">
                        </div>
                    </div>
          
                                  <div class="form-group">
                        <label for="price" class="col-sm-3 control-label">邮箱</label>
                        <div class="col-sm-9">
                            <input type="text" class="form-control" name="price"  id="book_delete_price"
                                   placeholder="价格">
                        </div>
                    </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                <button onclick = "onDeleteUserSubmitButtonClicked()" class="btn btn-primary">
                    提交
                </button><span id="book_delete_submit_btn"> </span>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>

<!-- 用户列表 -->
<div id = "books_div" class="panel panel-default">
    <div class="panel-heading">
        <h3 class="panel-title">
            用户列表：
        </h3>
    </div>
    <div id = "books_list" class="panel-body">
        <table id = "books_table" class="table table-hover">
            <thead>
            <tr>
                <th>用户名</th>
                <th>密码</th>
                <th>昵称</th>
               	<th>姓名</th>
                <th>性别</th>
                <th>邮箱</th>
                <th>添加好友</th>
                <th>共同好友</th>
            </tr>
            </thead>
            <tbody id = "books_tbody">
            <c:forEach items="${books}" var="book">
                <tr id = "book${book.user_id }">
                    <td>${book.user_id }</td>
   					<td>${book.password}</td>
                   	<td>${book.nickname}</td>
                   	<td>${book.fullname}</td>
                   	<td>${book.sex}</td>
                   	<td>${book.email }</td>
                   	<td><a><button type="button" class="btn btn-success btn-xs" data-toggle="modal" data-target="#detailbook" onclick="onDetailBookButtonClicked(${book.user_id})">详情</button></a></td>
                   	<td><a><button type="button" class="btn btn-success btn-xs" data-toggle="modal" data-target="#deletebook" onclick="onQueryCommonFriendButtonClicked(${book.user_id})">共同好友</button></a></td>
                </tr>
            </c:forEach>
            </tbody>
        </table>
    </div>
</div>


<!-- 好友列表 -->
<div id = "friends_div" class="panel panel-default">
    <div class="panel-heading">
        <h3 class="panel-title">
            好友列表：
        </h3>
    </div>
    <div id = "friends_list" class="panel-body">
        <table id = "friends_table" class="table table-hover">
            <thead>
            <tr>
                <th>所在分组</th>
                <th>用户名</th>
                <th>密码</th>
                <th>昵称</th>
               	<th>姓名</th>
                <th>性别</th>
                <th>邮箱</th>
                <th>发送消息</th>
                <th>删除好友</th>
                <th>共同好友</th>
            </tr>
            </thead>
            <tbody id = "friends_tbody">
            <c:forEach items="${friends}" var="friend">
                <tr id = "friend${friend.user_id }">
                </tr>
            </c:forEach>
            </tbody>
        </table>
    </div>
</div>
<!-- 消息列表 -->
<div id = "messages_div" class="panel panel-default">
    <div class="panel-heading">
        <h3 class="panel-title">
            消息列表：
        </h3>
    </div>
    <div id = "messages_list" class="panel-body">
        <table id = "messages_table" class="table table-hover">
            <thead>
            <tr>
                <th>消息id</th>
                <th>发送方</th>
                <th>接收方</th>
                <th>详情</th>
                <th>回复</th>
                <th>标为已读</th>
            </tr>
            </thead>
            <tbody id = "messages_tbody">
            <c:forEach items="${messages}" var="message">
                <tr id = "message${message.message_id }">
                </tr>
            </c:forEach>
            </tbody>
        </table>
    </div>
</div>

<!-- 好友请求 -->
<div id = "friend_requests_div" class="panel panel-default">
    <div class="panel-heading">
        <h3 class="panel-title">
            好友请求：
        </h3>
    </div>
    <div id = "friend_requests_list" class="panel-body">
        <table id = "friend_requests_table" class="table table-hover">
            <thead>
            <tr>
                <th>请求id</th>
                <th>发送方</th>
                <th>操作</th>
            </tr>
            </thead>
            <tbody id = "friend_requests_tbody">
            <c:forEach items="${friend_requests}" var="friend_request">
                <tr id = "friend_request${friend_request.request_id }">
                </tr>
            </c:forEach>
            </tbody>
        </table>
    </div>
</div>

<!-- 分组列表 -->
<div id = "groups_div" class="panel panel-default">
    <div class="panel-heading">
        <h3 class="panel-title">
            好友请求：
        </h3>
    </div>
    <div id = "groups_list" class="panel-body">
        <table id = "groups_table" class="table table-hover">
            <thead>
            <tr>
                <th>分组名</th>
                <th>好友数</th>
                <th>操作</th>
            </tr>
            </thead>
            <tbody id = "groups_tbody">
            <c:forEach items="${groups}" var="group">
                <tr id = "group${group.group_id }">
                </tr>
            </c:forEach>
            </tbody>
        </table>
    </div>
</div>


<!--  
<script> 
     //绑定模态框展示的方法 
    $('#modifybook').on('show.bs.modal', function (event) {  
        var button = $(event.relatedTarget) // 触发事件的按钮  
        var modal = $(this)  //获得模态框本身
        modal.find('.modal-body input').eq(0).val(button.data('title'))  
        modal.find('.modal-body input').eq(1).val(button.data('isbn'))
        modal.find('.modal-body input').eq(2).val(button.data('authorid'))
        modal.find('.modal-body input').eq(3).val(button.data('publisher'))
        modal.find('.modal-body input').eq(4).val(button.data('publishdate'))
        modal.find('.modal-body input').eq(5).val(button.data('price'))
    })  
</script>  
-->


<script type="text/javascript">
	function onListUserButtonClicked(){
        alert("hello");
		$.ajax({
			type : "get",
            url : "query_all_users",
 			data:{
                'user_id': $("#logining_user_id").val()
 			},
			dataType: "json",
			success: function(data){
				var books_tbody = $("#books_tbody tr");
				for(var i = 0; i < books_tbody.length; ++i){
					books_tbody.eq(i).remove();
				}
				for(var i = 0; i < data.length; ++i){
				     var book = data[i];
                    if(book.user_id == $("#logining_user_id").val()) {
                        continue;
                    }
                     if(book.is_friend == 0) {
                         $("#books_table tbody").prepend("<tr id=\"book" + book.user_id + "\">" + 
                                 "<td>" + book.user_id + "</td>" + 
                                 "<td>" + book.password + "</td>" + 
                                 "<td>" + book.nickname + "</td>" + 
                                 "<td>" + book.fullname + "</td>" + 
                                 "<td>" + book.sex + "</td>" + 
                                 "<td>" + book.email + "</td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#send_friend_request\" onclick=\"onSendFriendRequestButtonClicked(\'" + book.user_id + "\')\"" + ">加为好友</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#query_common_friend\" onclick=\"onQueryCommonFriendButtonClicked(\'" + book.user_id + "\')\"" + ">共同好友</button></a></td>" + 
                                 "</tr>");
                     }
                     else {
                         $("#books_table tbody").prepend("<tr id=\"book" + book.user_id + "\">" + 
                                 "<td>" + book.user_id + "</td>" + 
                                 "<td>" + book.password + "</td>" + 
                                 "<td>" + book.nickname + "</td>" + 
                                 "<td>" + book.fullname + "</td>" + 
                                 "<td>" + book.sex + "</td>" + 
                                 "<td>" + book.email + "</td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#send_friend_request\">已是好友</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#query_common_friend\" onclick=\"onQueryCommonFriendButtonClicked(\'" + book.user_id + "\')\"" + ">共同好友</button></a></td>" + 
                                 "</tr>");

                     }
				}
				if(!$("#messages_div").is(":hidden"))
					$("#messages_div").hide();
				if(!$("#friends_div").is(":hidden"))
					$("#friends_div").hide();
				if(!$("#friend_requests_div").is(":hidden"))
					$("#friend_requests_div").hide();
				if(!$("#groups_div").is(":hidden"))
					$("#groups_div").hide();

				if($("#books_div").is(":hidden"))
					$("#books_div").show();
			},
			error: function(){
				alert("error")

			}
		});
	}
</script>


<script type="text/javascript">
	function onQueryCommonFriendButtonClicked(friend_id){
        alert("query common friend");
		$.ajax({
 			type: "get",
            url: "query_common_friend",
 			data:{
                'user_id': $("#logining_user_id").val(),
                'friend_id': friend_id
 			},
			success: function(data1){
				var books_tbody = $("#books_tbody tr");
				for(var i = 0; i < books_tbody.length; ++i){
					books_tbody.eq(i).remove();
				}
				var data = eval('(' + data1 + ')');
                alert(data.length);
				for(var i = 0; i < data.length; ++i){
				     var book = data[i];
					 $("#books_table tbody").prepend("<tr id=\"book" + book.user_id + "\">" + 
	 						 "<td>" + book.user_id + "</td>" + 
	 						 "<td>" + book.password + "</td>" + 
	 						 "<td>" + book.nickname + "</td>" + 
	 						 "<td>" + book.fullname + "</td>" + 
	 						 "<td>" + book.sex + "</td>" + 
	 						 "<td>" + book.email + "</td>" + 
	 						 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#send_friend_request\" onclick=\"onSendFriendRequestButtonClicked(\'" + book.user_id + "\')\"" + ">加为好友</button></a></td>" + 
	 						 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#modifybook\" onclick=\"onModifyUserButtonClicked(\'" + book.user_id + "\')\"" + ">修改</button></a></td>" + 
	                       	 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#deletebook\" onclick=\"onDeleteUserButtonClicked(\'" + book.user_id + "\')\"" + ">删除</button></a></td>" + 
	                       	 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#query_common_friend\" onclick=\"onQueryCommonFriendButtonClicked(\'" + book.user_id + "\')\"" + ">共同好友</button></a></td>" + 
	 						 "</tr>");
				}

				if(!$("#messages_div").is(":hidden"))
					$("#messages_div").hide();
				if(!$("#friends_div").is(":hidden"))
					$("#friends_div").hide();
				if(!$("#friend_requests_div").is(":hidden"))
					$("#friend_requests_div").hide();
				if(!$("#groups_div").is(":hidden"))
					$("#groups_div").hide();

				if($("#books_div").is(":hidden"))
					$("#books_div").show();
			},
			error: function(){
				alert("error")
			},
			complete: function(){
				alert("complete")
			}
		});
	}
</script>

<script type="text/javascript">
	function onListGroupButtonClicked(){
		$.ajax({
			type : "get",
            url : "query_all_groups",
 			data:{
                'user_id': $("#logining_user_id").val()
 			},
			dataType: "json",
			success: function(data){
                if (!data && typeof(data)!="undefined" && data!=0){   
                }  
                else {
                    var books_tbody = $("#groups_tbody tr");
                    for(var i = 0; i < books_tbody.length; ++i){
                        books_tbody.eq(i).remove();
                    }
                    for(var i = 0; i < data.length; ++i){
                         var group = data[i];
                         $("#groups_table tbody").prepend("<tr id=\"group" + group.group_id + "\">" + 
                                 "<td>" + group.group_name + "</td>" + 
                                 "<td>" + group.friend_count + "</td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#modify_group\" onclick=\"onModifyGroupNameButtonClicked(\'" + group.group_id + "\',\'" + group.group_name + "\')\"" + ">修改分组名</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#delete_group\" onclick=\"onDeleteGroupButtonClicked(\'" + group.group_id + "\',\'" + group.group_name + "\',\'" + group.friend_count + "\')\"" + ">删除分组</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" onclick=\"onListFriendsInGroupButtonClicked(\'" + group.group_id + "\',\'" + group.group_name + "\')\"" + ">所有好友</button></a></td>" + 
                                 "</tr>");
                    }
                 }
				if(!$("#messages_div").is(":hidden"))
					$("#messages_div").hide();
				if(!$("#books_div").is(":hidden"))
					$("#books_div").hide();
				if(!$("#friend_requests_div").is(":hidden"))
					$("#friend_requests_div").hide();
				if(!$("#friends_div").is(":hidden"))
					$("#friends_div").hide();

				if($("#groups_div").is(":hidden"))
					$("#groups_div").show();
			},
			error: function(){
				alert("error")

			}
		});
	}
</script>
<script type="text/javascript">
	function onListFriendButtonClicked(){
		$.ajax({
			type : "get",
            url : "query_all_friends",
 			data:{
                'user_id': $("#logining_user_id").val()
 			},
			dataType: "json",
			success: function(data){
                if (!data && typeof(data)!="undefined" && data!=0){   
                }  
                else {
                    var books_tbody = $("#friends_tbody tr");
                    for(var i = 0; i < books_tbody.length; ++i){
                        books_tbody.eq(i).remove();
                    }
                    for(var i = 0; i < data.length; ++i){
                         var friend = data[i];
                         $("#friends_table tbody").prepend("<tr id=\"friend" + friend.user_id + "\">" + 
                                 "<td>" + friend.group_name + "</td>" + 
                                 "<td>" + friend.user_id + "</td>" + 
                                 "<td>" + friend.password + "</td>" + 
                                 "<td>" + friend.nickname + "</td>" + 
                                 "<td>" + friend.fullname + "</td>" + 
                                 "<td>" + friend.sex + "</td>" + 
                                 "<td>" + friend.email + "</td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#send_message\" onclick=\"onSendMessageButtonClicked(\'" + friend.user_id + "\')\"" + ">发送消息</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#send_message\" onclick=\"onDetailUserButtonClicked(\'" + friend.user_id + "\')\"" + ">详细信息</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#delete_friend\" onclick=\"onDeleteFriendButtonClicked(\'" + friend.user_id + "\')\"" + ">删除好友</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#modifyfriendgroup\" onclick=\"onModifyFriendGroupButtonClicked(\'" + friend.group_name + "\',\'" + friend.user_id + "\')\"" + ">修改分组</button></a></td>" + 
                                 "</tr>");
                    }
                }
				if(!$("#messages_div").is(":hidden"))
					$("#messages_div").hide();
				if(!$("#books_div").is(":hidden"))
					$("#books_div").hide();
				if(!$("#friend_requests_div").is(":hidden"))
					$("#friend_requests_div").hide();
				if(!$("#groups_div").is(":hidden"))
					$("#groups_div").hide();

				if($("#friends_div").is(":hidden"))
					$("#friends_div").show();

			},
			error: function(){
				alert("error")
			}
		});
	}
</script>

<script type="text/javascript">
	function onListFriendsInGroupButtonClicked(group_id, group_name){
		$.ajax({
			type : "get",
            url : "query_friends_in_group",
 			data:{
                'user_id': $("#logining_user_id").val(),
                'group_id': group_id
 			},
			dataType: "json",
			success: function(data){
                var books_tbody = $("#friends_tbody tr");
                for(var i = 0; i < books_tbody.length; ++i){
                    books_tbody.eq(i).remove();
                }
                if (!data && typeof(data)!="undefined" && data!=0){   
                }
                else {
                    for(var i = 0; i < data.length; ++i){
                         var friend = data[i];
                         $("#friends_table tbody").prepend("<tr id=\"friend" + friend.user_id + "\">" + 
                                 "<td>" + group_name + "</td>" + 
                                 "<td>" + friend.user_id + "</td>" + 
                                 "<td>" + friend.password + "</td>" + 
                                 "<td>" + friend.nickname + "</td>" + 
                                 "<td>" + friend.fullname + "</td>" + 
                                 "<td>" + friend.sex + "</td>" + 
                                 "<td>" + friend.email + "</td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#send_message\" onclick=\"onSendMessageButtonClicked(\'" + friend.user_id + "\')\"" + ">发送消息</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#send_message\" onclick=\"onDetailUserButtonClicked(\'" + friend.user_id + "\')\"" + ">详细信息</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#delete_friend\" onclick=\"onDeleteFriendButtonClicked(\'" + friend.user_id + "\')\"" + ">删除好友</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#modifyfriendgroup\" onclick=\"onModifyFriendGroupButtonClicked(\'" + friend.group_name + "\',\'" + friend.user_id + "\')\"" + ">修改分组</button></a></td>" + 
                                 "</tr>");
                    }
                }
				if(!$("#messages_div").is(":hidden"))
					$("#messages_div").hide();
				if(!$("#books_div").is(":hidden"))
					$("#books_div").hide();
				if(!$("#friend_requests_div").is(":hidden"))
					$("#friend_requests_div").hide();
				if(!$("#groups_div").is(":hidden"))
					$("#groups_div").hide();

				if($("#friends_div").is(":hidden"))
					$("#friends_div").show();

			},
			error: function(){
				alert("error")
			}
		});
	}
</script>
<script type="text/javascript">
	function onListFriendRequestButtonClicked(){
		$.ajax({
 			type: "get",
            url: "query_friend_request",
 			data:{
                'user_id': $("#logining_user_id").val()
 			},
            dataType: 'json',
			success: function(data){
                if (!data && typeof(data)!="undefined" && data!=0){   
                }  
                else {
                    var books_tbody = $("#friend_requests_tbody tr");
                    for(var i = 0; i < books_tbody.length; ++i){
                        books_tbody.eq(i).remove();
                    }
                    for(var i = 0; i < data.length; ++i){
                         var book = data[i];
                         $("#friend_requests_table tbody").prepend("<tr id=\"friend_request" + book.request_id + "\">" + 
                                 "<td>" + book.request_id + "</td>" + 
                                 "<td>" + book.sender_id + "</td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#send_friend_request\" onclick=\"onAgreeFriendRequestButtonClicked(\'" + book.request_id + "\',\'" + book.sender_id + "\',\'" + book.recver_id + "\',\'" + book.sender_group + "\')\"" + ">同意</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#send_friend_request\" onclick=\"onRejectFriendRequestButtonClicked(\'" + book.request_id + "\')\">拒绝</button></a></td>" + 
                                 "</tr>");
                    }
                }
				if(!$("#messages_div").is(":hidden"))
					$("#messages_div").hide();
				if(!$("#books_div").is(":hidden"))
					$("#books_div").hide();
				if(!$("#groups_div").is(":hidden"))
					$("#groups_div").hide();
				if(!$("#friends_div").is(":hidden"))
					$("#friends_div").hide();

				if($("#friend_requests_div").is(":hidden"))
					$("#friend_requests_div").show();

			},
			error: function(){
				alert("error")
			},
			complete: function(){
			}
		});
	}
</script>
<script type="text/javascript">
	function onReadDoneMessageButtonClicked(message_id, sender_id, recver_id, context){
		$.ajax({
 			type: "post",
            url: "read_done_message",
 			data:{
                'user_id': recver_id,
                'message_id': message_id
 			},
			success: function(data){
                alert("reply done");
                $("#message"+message_id).remove();
			},
			error: function(){
				alert("error")
			},
			complete: function(){
				alert("complete");
			}
		});
	}
</script>
<script type="text/javascript">
	function onReplyMessageButtonClicked(message_id, sender_id, recver_id, context){
        $("#reply_message_message_id").val(message_id);
        $("#reply_message_sender").val(sender_id);
        $("#reply_message_recver").val(recver_id);
        $("#reply_message_context").val(context);

        $("#reply_message_message_id").attr("readonly", "readonly");
        $("#reply_message_sender").attr("readonly", "readonly");
        $("#reply_message_recver").attr("readonly", "readonly");
        $("#reply_message_context").attr("readonly", "readonly");
	}
</script>

<script type="text/javascript">
	function onReplyMessageSubmitButtonClicked(){
		$.ajax({
 			type: "post",
            url: "reply_message",
 			data:{
                'message_id': $("#reply_message_message_id").val(),
                'sender_id': $("#reply_message_recver").val(),
                'recver_id': $("#reply_message_sender").val(),
                'context': $("#reply_message_reply_context").val()
 			},
			success: function(data){
                alert("reply done");
			},
			error: function(){
				alert("error")
			},
			complete: function(){
				alert("complete");
                $("#reply_message").modal('hide');
			}
		});
	}
</script>

<script type="text/javascript">
	function onSendMessageButtonClicked(friend_id){
        $("#send_message_user_id").val($("#logining_user_id").val());
        $("#send_message_friend_id").val(friend_id);
        $("#send_message_user_id").attr("readonly", "readonly"),
        $("#send_message_friend_id").attr("readonly", "readonly")
	}
</script>


<script type="text/javascript">
	function onSendMessageSubmitButtonClicked(){
		$.ajax({
 			type: "post",
            url: "send_message",
 			data:{
                'user_id': $("#logining_user_id").val(),
                'friend_id': $("#send_message_friend_id").val(),
                'context': $("#send_message_context").val()
 			},
			success: function(data){
                alert("send done");
			},
			error: function(){
				alert("error")
			},
			complete: function(){
				alert("complete");
                $("#send_message").modal('hide');
			}
		});
	}
</script>


<script type="text/javascript">
	function onDeleteGroupButtonClicked(group_id, group_name, friend_counts){
        alert(friend_counts);
        $("#delete_group_group_id").val(group_id);
        $("#delete_group_group_name").val(group_name);
        $("#delete_group_friend_counts").val(friend_counts);
	}
</script>

<script type="text/javascript">
	function onDeleteGroupSubmitButtonClicked(){
		$.ajax({
			type: 'post',
            url : 'delete_group',
			dataType : "json",
			data: {
				user_id : $("#logining_user_id").val(),
                group_id: $("#delete_group_group_id").val(),
			},
			success: function(data){
                $("#group"+group_id).remove();
			},
			error: function(){				
				alert("delete error");
			},
			complete: function(){
				$("#delete_group").modal('hide')
			}
		});
	}
</script>
<script type="text/javascript">
	function onModifyGroupNameButtonClicked(group_id, group_name){
        $("#modify_group_group_id").val(group_id);
        $("#modify_group_group_name").val(group_name);
	}
</script>

<script type="text/javascript">
	function onModifyGroupNameSubmitButtonClicked(){
		$.ajax({
			type: 'post',
            url : 'modify_group_name',
			dataType : "json",
			data: {
				user_id : $("#logining_user_id").val(),
                group_id: $("#modify_group_group_id").val(),
                group_name: $("#modify_group_group_name").val()
			},
			success: function(data){
                var group_id = $("#modify_group_group_id").val();
                var group_name = $("#modify_group_group_name").val();
                $("#group"+group_id).children().eq(0).text(group_name);
			},
			error: function(){				
				alert("modify error");
			},
			complete: function(){
				$("#modify_group").modal('hide')
			}
		});
	}
</script>
<script type="text/javascript">
	function onModifyFriendGroupButtonClicked(group_name, friend_id){
        $("#modify_friend_group_friend_id").val(friend_id);
        $("#modify_friend_group_group_name").val(group_name);
	}
</script>

<script type="text/javascript">
	function onModifyFriendGroupSubmitButtonClicked(){
		$.ajax({
			type: 'post',
            url : 'modify_friend_group',
			dataType : "json",
			data: {
				user_id : $("#logining_user_id").val(),
                friend_id: $("#modify_friend_group_friend_id").val(),
                group_name: $("#modify_friend_group_group_name").val()
			},
			success: function(data){
                var friend_id = data.user_id;
                $("#friend"+friend_id).children().eq(0).text(data.group_name);
                $("#friend"+friend_id).children().eq(1).text(data.user_id);
                $("#friend"+friend_id).children().eq(2).text(data.password);
                $("#friend"+friend_id).children().eq(3).text(data.nickname);
                $("#friend"+friend_id).children().eq(4).text(data.fullname);
                $("#friend"+friend_id).children().eq(5).text(data.sex);
                $("#friend"+friend_id).children().eq(6).text(data.email);
			},
			error: function(){				
				alert("modify error");
			},
			complete: function(){
				$("#modifyfriendgroup").modal('hide')
			}
		});
	}
</script>
<script type="text/javascript">
	function onModifyUserButtonClicked(){
		$.ajax({
			type: 'get',
            url : 'query_user_information',
			dataType : "json",
			data: {
				user_id : $("#logining_user_id").val()
			},
			success: function(data){
                $("#modify_user_user_id").val(data.user_id);
                $("#modify_user_password").val(data.password);
                $("#modify_user_nickname").val(data.nickname);
                $("#modify_user_fullname").val(data.fullname);
                $("#modify_user_sex").val(data.sex);
                $("#modify_user_email").val(data.email);

                $("#modify_user_user_id").attr("readonly", "readonly");
			},
			error: function(){				
				alert("modify error");
			},
			complete: function(){
				$("#modifybook").modal('hide')
			}
		});
	}
</script>
<script type="text/javascript">
	function onModifyUserSubmitButtonClicked(){
		$.ajax({
			type: 'post',
            url : 'modify_user',
			dataType : "json",
			data: {
				user_id : $("#modify_user_user_id").val(),
				password: $("#modify_user_password").val(),
				nickname: $("#modify_user_nickname").val(),
				fullname: $("#modify_user_fullname").val(),
				sex: $("#modify_user_sex").val(),
				email: $("#modify_user_email").val()
			},
			success: function(data){
				$("#book"+data.user_id).children().eq(0).text(data.user_id);
				$("#book"+data.user_id).children().eq(1).text(data.password);
				$("#book"+data.user_id).children().eq(2).text(data.nickname);
				$("#book"+data.user_id).children().eq(3).text(data.fullname);
				$("#book"+data.user_id).children().eq(4).text(data.sex);
				$("#book"+data.user_id).children().eq(5).text(data.email);

                $("#modifyuser").modal('hide');
			},
			error: function(){				
				alert("modify error");
			},
			complete: function(){
				$("#modifybook").modal('hide')
			}
		});
	}
</script>

<script type="text/javascript">
	function onUserLoginButtonClicked(){
		$("#user_login_id").val("");
		$("#user_login_password").val("");
	}
</script>

<script type="text/javascript">
 	function onUserLoginSubmitButtonClicked(){
 		if($("#user_login_id").val()=='' || $("#user_login_password").val()==''){
 			alert("请输入完整信息");
 			return false;
 		}
 		$.ajax({
 			type: "post",
            url: "user_login",
 			data:{
 				user_id: $("#user_login_id").val(),
                password: $("#user_login_password").val()
 			},
 			success: function(data){

				var obj = eval('(' + data + ')');
                // alert(obj)
				$("#logining_user_id").val(obj.user_id),
				$("#logining_user_password").val(obj.password),
				$("#logining_user_nickname").val(obj.nickname),
				$("#logining_user_fullname").val(obj.fullname),
				$("#logining_user_sex").val(obj.sex),
				$("#logining_user_email").val(obj.email),
				
				$("#logining_user_id").attr("readonly", "readonly"),
				$("#logining_user_password").attr("readonly", "readonly"),
				$("#logining_user_nickname").attr("readonly", "readonly"),
				$("#logining_user_fullname").attr("readonly", "readonly"),
				$("#logining_user_sex").attr("readonly", "readonly")
				$("#logining_user_email").attr("readonly", "readonly")
                
                // $("#logining_user").modal('show');

                //关键在这里，Ajax定时访问服务端，不断获取数据 ，这里是1秒请求一次。
                // window.setInterval(function(){$.ajax(getting(obj.user_id))},1000);
 			},
 			error: function(){
                alert("login error");
 			},
 			complete: function(){
				$("#userlogin").modal('hide');
 			}
 		});
 	}
</script>   

<script type="text/javascript">
	function getting(user_id){
        alert(user_id);
 		$.ajax({
 			type: "get",
            url: "query_friend_request",
 			data:{
                'user_id': user_id
 			},
            dataType: 'json',
 			success: function(data){
                alert(data);
 			},
 			error: function(){
                alert("error");
 			},
 			complete: function(){
                alert("complete");
 			}
 		});
	}
</script>


<script type="text/javascript">
	function onSendFriendRequestButtonClicked(friend_id){
        alert(friend_id)
        $("#choose_group_user_id").val($("#logining_user_id").val()),
        $("#choose_group_friend_id").val(friend_id),
        $("#choose_group_group_name").val(""),
        
        $("#choose_group_user_id").attr("readonly", "readonly"),
        $("#choose_group_friend_id").attr("readonly", "readonly")

        $("#choosegroup").modal('show');
	}
</script>

<script type="text/javascript">
	function onSendFriendRequestSubmitButtonClicked(){
		$.ajax({
 			type: "post",
            url: "send_friend_request",
 			data:{
                'user_id': $("#choose_group_user_id").val(),
                'friend_id': $("#choose_group_friend_id").val(),
                'group_name': $("#choose_group_group_name").val(),
 			},
			success: function(data){
                alert(data)
			},
			error: function(){
				alert("error")
			},
			complete: function(){
				alert("complete");
                $("#choosegroup").modal('hide');
			}
		});
	}
</script>

<script type="text/javascript">
 	function onAgreeFriendRequestButtonClicked(request_id, sender, recver, sender_group){
        $("#friend_request_id").val(request_id);
        $("#friend_request_sender").val(sender);
        $("#friend_request_recver").val(recver);
        $("#friend_request_sender_group").val(sender_group);

        $("#friend_request_id").attr("readonly", "readonly"),
        $("#friend_request_sender").attr("readonly", "readonly")
        $("#friend_request_recver").attr("readonly", "readonly")
        $("#friend_request_sender_group").attr("readonly", "readonly")

        $("#friendrequest").modal('show');
 	}
</script>   

<script type="text/javascript">
 	function onAgreeFriendRequestSubmitButtonClicked(){
 		$.ajax({
 			type: "post",
            url: "agree_friend_request",
 			data:{
                request_id: $("#friend_request_id").val(),
                sender_id: $("#friend_request_sender").val(),
                recver_id: $("#friend_request_recver").val(),
                sender_group: $("#friend_request_sender_group").val(),
                recver_group: $("#friend_request_group_name").val(),
 			},
 			success: function(data){
                alert(data);
 			},
 			error: function(){
                alert("error");
 			},
 			complete: function(){
                alert("complete");
				$("#friendrequest").modal('hide');
 			}
 		});
 	}
</script>   

<script type="text/javascript">
 	function onRejectFriendRequestButtonClicked(request_id){
        alert(request_id);
 		$.ajax({
 			type: "post",
            url: "reject_friend_request",
 			data:{
                'user_id': $("#logining_user_id").val(),
                'request_id': request_id
 			},
 			success: function(data){
				$("#request_friend"+ request_id).remove();
                alert(data);
 			},
 			error: function(){
                alert("error");
 			},
 			complete: function(){
                alert("complete");
				$("#friendrequest").modal('hide');
 			}
 		});
 	}
</script>   

<script type="text/javascript">
 	function onQueryChatMessageButtonClicked(){
 		$.ajax({
 			type: "get",
            url: "query_chat_message",
 			data:{
                user_id: $("#logining_user_id").val()
 			},
			dataType: "json",
 			success: function(data){
                if (!data && typeof(data)!="undefined" && data!=0){   
                }  
                else {
                    var messages_tbody = $("#messages_tbody tr");
                    for(var i = 0; i < messages_tbody.length; ++i){
                        messages_tbody.eq(i).remove();
                    }
                    for(var i = 0; i < data.length; ++i){
                         var message = data[i];
                         $("#messages_table tbody").prepend("<tr id=\"message" + message.message_id + "\">" + 
                                 "<td>" + message.message_id + "</td>" + 
                                 "<td>" + message.sender_id + "</td>" + 
                                 "<td>" + message.recver_id + "</td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#detailmessage\" onclick=\"onDetailMessageButtonClicked(\'" + message.message_id + "\',\'" + message.sender_id + "\',\'" + message.recver_id + "\',\'" + message.context + "\')\"" + ">详情</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#reply_message\" onclick=\"onReplyMessageButtonClicked(\'" + message.message_id + "\',\'" + message.sender_id + "\',\'" + message.recver_id + "\',\'" + message.context + "\')\"" + ">回复</button></a></td>" + 
                                 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" onclick=\"onReadDoneMessageButtonClicked(\'" + message.message_id + "\',\'" + message.sender_id + "\',\'" + message.recver_id + "\',\'" + message.context + "\')\"" + ">标为已读</button></a></td>" + 
                                 "</tr>");
                    }
                }
				if(!$("#books_div").is(":hidden"))
					$("#books_div").hide();
				if(!$("#groups_div").is(":hidden"))
					$("#groups_div").hide();
				if(!$("#friends_div").is(":hidden"))
					$("#friends_div").hide();
				if(!$("#friend_requests_div").is(":hidden"))
					$("#friend_requests_div").hide();

                // alert(data);
                $("#messages_div").show();
 			},
 			error: function(){
                alert("error");
 			},
 			complete: function(){
 			}
 		});
 	}
</script>   

<script type="text/javascript">
	function onDetailMessageButtonClicked(id, sender_id, recver_id, context){
        alert("hello");
        $("#recv_message_user_id").val(recver_id);
        $("#recv_message_friend_id").val(sender_id);
        $("#recv_message_context").val(context);

        $("#recv_message_user_id").attr("readonly", "readonly"),
        $("#recv_message_friend_id").attr("readonly", "readonly")
        $("#recv_message_context").attr("readonly", "readonly")
	}
</script>


<script type="text/javascript">
	function onQueryUserButtonClicked(){
		$("#book_query_title").val("");
	}
	
</script>


<script type="text/javascript">
 	function onQueryUserSubmitButtonClicked(){
 		if($("#book_query_title").val()==''){
 			alert("请输入完整信息");
 			return false;
 		}
 		$.ajax({
 			type: "get",
            url: "query_user",
 			data:{
 				user_id: $("#book_query_title").val()
 			},
 			dataType: "json",
 			success: function(data){
 						
 				var book_tbody = $("#books_tbody tr");
 				
 				for(var i = 0; i < book_tbody.length; ++i){
 						book_tbody.eq(i).remove();
 				}
 				
 				var book = data;
				 $("#books_table tbody").prepend("<tr id=\"book" + book.isbn + "\">" + 
 						 "<td>" + book.user_id + "</td>" + 
 						 "<td>" + book.password + "</td>" + 
 						 "<td>" + book.nickname + "</td>" + 
 						 "<td>" + book.fullname + "</td>" + 
 						 "<td>" + book.sex + "</td>" + 
 						 "<td>" + book.email + "</td>" + 
 						 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#detailbook\" onclick=\"onDetailBookButtonClicked(" + book.user_id + ")\"" + ">详情</button></a></td>" + 
 						 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#modifyuser\" onclick=\"onModifyUserButtonClicked(" + book.user_id + ")\"" + ">修改</button></a></td>" + 
                       	 "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#deletebook\" onclick=\"onDeleteUserButtonClicked(" + book.user_id + ")\"" + ">删除</button></a></td>" + 
 						 "</tr>");
				 
 			
 			},
 			error: function(){
 				var book_tbody = $("#books_tbody tr");
 				for(var i = 0; i < book_tbody.length; ++i){
						book_tbody.eq(i).remove();
				}
 			},
 			complete: function(){
				$("#querybook").modal('hide');
 				
				if(!$("#authors_div").is(":hidden"))
					$("#authors_div").hide();
				if($("#books_div").is(":hidden"))
					$("#books_div").show();
				
 			}
 		});
 	}
 
 </script>   


<script type="text/javascript">
	function onDeleteUserButtonClicked(user_id){
		$.ajax({
			type: 'get',
			data :{
				"user_id": user_id
			},
			url : "query_user",
			success: function(data){
				var obj = eval('(' + data + ')');
				$("#book_delete_title").val(obj.user_id),
				$("#book_delete_authorID").val(obj.password),
				$("#book_delete_isbn").val(obj.nickname),
				$("#book_delete_publisher").val(obj.fullname),
				$("#book_delete_publishDate").val(obj.sex),
				$("#book_delete_price").val(obj.email),
				
				$("#book_delete_title").attr("readonly", "readonly"),
				$("#book_delete_authorID").attr("readonly", "readonly"),
				$("#book_delete_isbn").attr("readonly", "readonly"),
				$("#book_delete_publisher").attr("readonly", "readonly"),
				$("#book_delete_publishDate").attr("readonly", "readonly"),
				$("#book_delete_price").attr("readonly", "readonly")
			},
			error: function(){
				alert("delete error")
			},
			complete: function(){
				
			}
		});
	}
</script>

<script type="text/javascript">
	function onDeleteFriendButtonClicked(friend_id){
        alert(friend_id);
		$.ajax({
			type: 'post',
			data :{
				"user_id": $("#logining_user_id").val(),
                'friend_id': friend_id
			},
			url : "delete_friend",
            dataType: 'json',
			success: function(data){
                $("#friend"+friend_id).remove();
			},
			error: function(){
				alert("delete error")
			},
			complete: function(){
				
			}
		});
	}
</script>
<script type="text/javascript">
	function onDeleteUserSubmitButtonClicked(){
		
		var delete_user_id = $("#book_delete_title").val();
		$.ajax({
			type: 'post',
			url : 'remove_user',
			data: {
				user_id: delete_user_id
			},
			success: function(){
				$("#book"+ delete_user_id).remove();
			},
			error: function(){
				alert("delete error");
			},
			complete: function(){
				$("#deletebook").modal('hide')
			}
		});
	}
</script>

<script type="text/javascript">
	function onAddGroupButtonClicked(){
        alert("hello")
		$("#group_add_name").val("");
	}
</script>

<script type="text/javascript">
	function onAddGroupSubmitButtonClicked(){
		$.ajax({
			type: "post",
			url : "add_group",
			dataType: "json",
			data: {
				user_id : $("#logining_user_id").val(),
                group_name: $("#group_add_name").val()
			},
			success: function(data){
                if(data.hasOwnProperty("reason")) {
                    alert(data["reason"]);
                }
                else {
                    alert("ok");
                }
			},
			error: function(){
				alert("format error");
			},
			complete: function(){
				$("#addgroup").modal('hide');
			}
		});
	}
</script>
<script type="text/javascript">
	function onAddUserButtonClicked(){
        alert("hello")
		$("#book_add_title").val("");
		$("#book_add_isbn").val("");
		$("#book_add_authorID").val("");
		$("#book_add_publisher").val("");
		$("#book_add_publishDate").val("");
		$("#book_add_price").val("");
	}
</script>

<script type="text/javascript">
	function onAddUserSubmitButtonClicked(){
        alert("register user");
		$.ajax({
			type: "post",
			url : "register_user",
			dataType: "json",
			data: {
				user_id : $("#book_add_title").val(),
				password: $("#book_add_isbn").val(),
				nickname: $("#book_add_authorID").val(),
				fullname: $("#book_add_publisher").val(),
				sex: $("#book_add_publishDate").val(),
				email: $("#book_add_price").val()
			},
			success: function(data){
                if(data.hasOwnProperty('reason')) {
                    alert(data.reason);
                }
                else {
                    var book = data;
                    $("#books_table tbody").prepend("<tr id=\"book" + book.user_id + "\">" + 
                                                     "<td>" + book.user_id + "</td>" + 
                                                     "<td>" + book.password + "</td>" + 
                                                     "<td>" + book.nickname + "</td>" + 
                                                     "<td>" + book.fullname + "</td>" + 
                                                     "<td>" + book.sex + "</td>" + 
                                                     "<td>" + book.email + "</td>" + 
                                                     "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#detailbook\" onclick=\"onDetailBookButtonClicked(\'" + book.user_id + "\')\"" + ">详情</button></a></td>" + 
                                                     "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#modifyuser\" onclick=\"onModifyUserButtonClicked(\'" + book.user_id + "\')\"" + ">修改</button></a></td>" + 
                                                     "<td><a><button type=\"button\" class=\"btn btn-success btn-xs\" data-toggle=\"modal\" data-target=\"#deletebook\" onclick=\"onDeleteUserButtonClicked(\'" + book.user_id + "\')\"" + ">删除</button></a></td>" + 
                                                     "</tr>");
                }
			},
			error: function(){
			
				alert("format error");
			},
			complete: function(){
				$("#addbook").modal('hide');
			}
		});
	}
</script>

<script type="text/javascript">

function onAddAuthorButtonClicked(){
	$("#author_add_authorID").val("");
	$("#author_add_name").val("");
	$("#author_add_age").val("");
	$("#author_add_country").val("");
}
</script>

<script type="text/javascript">
	$("#addauthor").on('shown.bs.modal',function(e){
		onAddAuthorButtonClicked();
	})
</script>


</body>
</html>
