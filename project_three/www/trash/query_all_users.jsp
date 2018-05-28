<!DOCTYPE html>
<html>
<head>
	

    <link rel="stylesheet" href="/www/css/bootstrap.min.css">
    <script src="/www/js/jquery.min.js"></script>
    <script src="/www/js/bootstrap.min.js"></script>
	
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">  
	
    <script>
        function func(friend_id){
            document.getElementById("demo").innerHTML="Hello World";
            $.ajax({
                type: "post",
                url: "add_friend",
                dataType: "json",
                data :{
                    'user_id' : main_user_id
                    'friend_id' : friend_id
                },
                
                success: function(data){
                    document.getElementById("demo").innerHTML="success";
                },
                error: function(){
                    document.getElementById("demo").innerHTML="error";
                },
                complete: function(){
                }
            });
        }
    </script>
</head>
<body>

                <li>{{main_user_id}}</li>

<div id = "users_div" class="panel panel-default">
    <div class="panel-heading">
        <h3 class="panel-title">
            用户列表：
        </h3>
    </div>
    <div id = "users_list" class="panel-body">
        <table id = "users_table" class="table table-hover">
            <thead>
            <tr>
                <th>用户名</th>
                <th>昵称</th>
                <th>姓名</th>
               	<th>性别</th>
                <th>邮箱</th>
            </tr>
            </thead>
            <tbody id = "users_tbody">
            {% for user in list %}
                <tr id = "user${user.isbn }">
                    <td>{{user.user_id }}</td>
                    <td>{{user.nickname}}</td>
                    <td>{{user.fullname}}</td>
                    <td>{{user.sex }}</td>
                    <td>{{user.email}}</td>
                    <td><a><button onclick="func('{{user.user_id}}')">加为好友</button></a></td>
                </tr>
            {% endfor %}
            </tbody>
        </table>
    </div>
</div>



</body>

	<script type="text/javascript" src="js/particles.js"></script>
	<script type="text/javascript" src="js/app.js"></script>
</html>
